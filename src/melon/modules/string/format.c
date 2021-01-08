/**
 * Based on Marco Paland's tiny printf.
 * https://github.com/mpaland/printf
 */

#include "melon/modules/string/format.h"
#include "melon/core/config.h"
#include "melon/core/tstring.h"
#include "melon/core/array.h"
#include "melon/core/utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// 'ntoa' conversion buffer size, this must be big enough to hold one converted
// numeric number including padded zeros (dynamically created on stack)
// default: 32 byte
#ifndef PRINTF_NTOA_BUFFER_SIZE
#define PRINTF_NTOA_BUFFER_SIZE 32U
#endif

// 'ftoa' conversion buffer size, this must be big enough to hold one converted
// float number including padded zeros (dynamically created on stack)
// default: 32 byte
#ifndef PRINTF_FTOA_BUFFER_SIZE
#define PRINTF_FTOA_BUFFER_SIZE 32U
#endif

// support for the floating point type (%f)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_FLOAT
#define PRINTF_SUPPORT_FLOAT
#endif

// support for exponential floating point notation (%e/%g)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#define PRINTF_SUPPORT_EXPONENTIAL
#endif

// define the default floating point precision
// default: 6 digits
#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
#define PRINTF_DEFAULT_FLOAT_PRECISION 6U
#endif

// define the largest float suitable to print with %f
// default: 1e9
#ifndef PRINTF_MAX_FLOAT
#define PRINTF_MAX_FLOAT 1e9
#endif

// support for the long long types (" MELON_PRINTF_SIZE " or %p)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_LONG_LONG
#define PRINTF_SUPPORT_LONG_LONG
#endif

// support for the ptrdiff_t type (%t)
// ptrdiff_t is normally defined in <stddef.h> as long or long long type
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_PTRDIFF_T
#define PRINTF_SUPPORT_PTRDIFF_T
#endif

///////////////////////////////////////////////////////////////////////////////

// internal flag definitions
#define FLAGS_ZEROPAD (1U << 0U)
#define FLAGS_LEFT (1U << 1U)
#define FLAGS_PLUS (1U << 2U)
#define FLAGS_SPACE (1U << 3U)
#define FLAGS_HASH (1U << 4U)
#define FLAGS_UPPERCASE (1U << 5U)
#define FLAGS_CHAR (1U << 6U)
#define FLAGS_SHORT (1U << 7U)
#define FLAGS_LONG (1U << 8U)
#define FLAGS_LONG_LONG (1U << 9U)
#define FLAGS_PRECISION (1U << 10U)
#define FLAGS_ADAPT_EXP (1U << 11U)

// import float.h for DBL_MAX
#if defined(PRINTF_SUPPORT_FLOAT)
#include <float.h>
#endif

// output function type
typedef void (*out_fct_type)(char character, void *buffer, size_t idx, size_t maxlen);

// wrapper (used as buffer) for output function type
typedef struct
{
	void (*fct)(char character, void *arg);
	void *arg;
} out_fct_wrap_type;

// internal secure strlen
// \return The length of the string (excluding the terminating 0) limited by 'maxsize'
static inline unsigned int _strnlen_s(const char *str, size_t maxsize)
{
	const char *s;
	for (s = str; *s && maxsize--; ++s)
		;
	return (unsigned int)(s - str);
}

// internal test if char is a digit (0-9)
// \return true if char is a digit
static inline bool _is_digit(char ch)
{
	return (ch >= '0') && (ch <= '9');
}

// internal ASCII string to unsigned int conversion
static unsigned int _atoi(const char **str)
{
	unsigned int i = 0U;
	while (_is_digit(**str))
	{
		i = i * 10U + (unsigned int)(*((*str)++) - '0');
	}
	return i;
}

// output the specified string in reverse, taking care of any zero-padding
static size_t _out_rev(out_fct_type out, char *buffer, size_t idx, size_t maxlen, const char *buf, size_t len, unsigned int width, unsigned int flags)
{
	const size_t start_idx = idx;

	// pad spaces up to given width
	if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD))
	{
		for (size_t i = len; i < width; i++)
		{
			out(' ', buffer, idx++, maxlen);
		}
	}

	// reverse string
	while (len)
	{
		out(buf[--len], buffer, idx++, maxlen);
	}

	// append pad spaces up to given width
	if (flags & FLAGS_LEFT)
	{
		while (idx - start_idx < width)
		{
			out(' ', buffer, idx++, maxlen);
		}
	}

	return idx;
}

// internal itoa format
static size_t _ntoa_format(out_fct_type out, char *buffer, size_t idx, size_t maxlen, char *buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags)
{
	// pad leading zeros
	if (!(flags & FLAGS_LEFT))
	{
		if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
		{
			width--;
		}
		while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE))
		{
			buf[len++] = '0';
		}
		while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE))
		{
			buf[len++] = '0';
		}
	}

	// handle hash
	if (flags & FLAGS_HASH)
	{
		if (!(flags & FLAGS_PRECISION) && len && ((len == prec) || (len == width)))
		{
			len--;
			if (len && (base == 16U))
			{
				len--;
			}
		}
		if ((base == 16U) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE))
		{
			buf[len++] = 'x';
		}
		else if ((base == 16U) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE))
		{
			buf[len++] = 'X';
		}
		else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE))
		{
			buf[len++] = 'b';
		}
		if (len < PRINTF_NTOA_BUFFER_SIZE)
		{
			buf[len++] = '0';
		}
	}

	if (len < PRINTF_NTOA_BUFFER_SIZE)
	{
		if (negative)
		{
			buf[len++] = '-';
		}
		else if (flags & FLAGS_PLUS)
		{
			buf[len++] = '+'; // ignore the space if the '+' exists
		}
		else if (flags & FLAGS_SPACE)
		{
			buf[len++] = ' ';
		}
	}

	return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

// internal itoa for 'long' type
static size_t _ntoa_long(out_fct_type out, char *buffer, size_t idx, size_t maxlen, unsigned long value, bool negative, unsigned long base, unsigned int prec, unsigned int width, unsigned int flags)
{
	char buf[PRINTF_NTOA_BUFFER_SIZE];
	size_t len = 0U;

	// no hash for 0 values
	if (!value)
	{
		flags &= ~FLAGS_HASH;
	}

	// write if precision != 0 and value is != 0
	if (!(flags & FLAGS_PRECISION) || value)
	{
		do
		{
			const char digit = (char)(value % base);
			buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
			value /= base;
		} while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
	}

	return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative, (unsigned int)base, prec, width, flags);
}

// internal itoa for 'long long' type
#if defined(PRINTF_SUPPORT_LONG_LONG)
static size_t _ntoa_long_long(out_fct_type out, char *buffer, size_t idx, size_t maxlen, unsigned long long value, bool negative, unsigned long long base, unsigned int prec, unsigned int width, unsigned int flags)
{
	char buf[PRINTF_NTOA_BUFFER_SIZE];
	size_t len = 0U;

	// no hash for 0 values
	if (!value)
	{
		flags &= ~FLAGS_HASH;
	}

	// write if precision != 0 and value is != 0
	if (!(flags & FLAGS_PRECISION) || value)
	{
		do
		{
			const char digit = (char)(value % base);
			buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
			value /= base;
		} while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
	}

	return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative, (unsigned int)base, prec, width, flags);
}
#endif // PRINTF_SUPPORT_LONG_LONG

#if defined(PRINTF_SUPPORT_FLOAT)

#if defined(PRINTF_SUPPORT_EXPONENTIAL)
// forward declaration so that _ftoa can switch to exp notation for values > PRINTF_MAX_FLOAT
static size_t _etoa(out_fct_type out, char *buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags);
#endif

// internal ftoa for fixed decimal floating point
static size_t _ftoa(out_fct_type out, char *buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags)
{
	char buf[PRINTF_FTOA_BUFFER_SIZE];
	size_t len = 0U;
	double diff = 0.0;

	// powers of 10
	static const double pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

	// test for special values
	if (value != value)
		return _out_rev(out, buffer, idx, maxlen, "nan", 3, width, flags);
	if (value < -DBL_MAX)
		return _out_rev(out, buffer, idx, maxlen, "fni-", 4, width, flags);
	if (value > DBL_MAX)
		return _out_rev(out, buffer, idx, maxlen, (flags & FLAGS_PLUS) ? "fni+" : "fni", (flags & FLAGS_PLUS) ? 4U : 3U, width, flags);

	// test for very large values
	// standard printf behavior is to print EVERY whole number digit -- which could be 100s of characters overflowing your buffers == bad
	if ((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT))
	{
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
		return _etoa(out, buffer, idx, maxlen, value, prec, width, flags);
#else
		return 0U;
#endif
	}

	// test for negative
	bool negative = false;
	if (value < 0)
	{
		negative = true;
		value = 0 - value;
	}

	// set default precision, if not set explicitly
	if (!(flags & FLAGS_PRECISION))
	{
		prec = PRINTF_DEFAULT_FLOAT_PRECISION;
	}
	// limit precision to 9, cause a prec >= 10 can lead to overflow errors
	while ((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 9U))
	{
		buf[len++] = '0';
		prec--;
	}

	int whole = (int)value;
	double tmp = (value - whole) * pow10[prec];
	unsigned long frac = (unsigned long)tmp;
	diff = tmp - frac;

	if (diff > 0.5)
	{
		++frac;
		// handle rollover, e.g. case 0.99 with prec 1 is 1.0
		if (frac >= pow10[prec])
		{
			frac = 0;
			++whole;
		}
	}
	else if (diff < 0.5)
	{
	}
	else if ((frac == 0U) || (frac & 1U))
	{
		// if halfway, round up if odd OR if last digit is 0
		++frac;
	}

	if (prec == 0U)
	{
		diff = value - (double)whole;
		if ((!(diff < 0.5) || (diff > 0.5)) && (whole & 1))
		{
			// exactly 0.5 and ODD, then round up
			// 1.5 -> 2, but 2.5 -> 2
			++whole;
		}
	}
	else
	{
		unsigned int count = prec;
		// now do fractional part, as an unsigned number
		while (len < PRINTF_FTOA_BUFFER_SIZE)
		{
			--count;
			buf[len++] = (char)(48U + (frac % 10U));
			if (!(frac /= 10U))
			{
				break;
			}
		}
		// add extra 0s
		while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0U))
		{
			buf[len++] = '0';
		}
		if (len < PRINTF_FTOA_BUFFER_SIZE)
		{
			// add decimal
			buf[len++] = '.';
		}
	}

	// do whole part, number is reversed
	while (len < PRINTF_FTOA_BUFFER_SIZE)
	{
		buf[len++] = (char)(48 + (whole % 10));
		if (!(whole /= 10))
		{
			break;
		}
	}

	// pad leading zeros
	if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD))
	{
		if (width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
		{
			width--;
		}
		while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE))
		{
			buf[len++] = '0';
		}
	}

	if (len < PRINTF_FTOA_BUFFER_SIZE)
	{
		if (negative)
		{
			buf[len++] = '-';
		}
		else if (flags & FLAGS_PLUS)
		{
			buf[len++] = '+'; // ignore the space if the '+' exists
		}
		else if (flags & FLAGS_SPACE)
		{
			buf[len++] = ' ';
		}
	}

	return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

#if defined(PRINTF_SUPPORT_EXPONENTIAL)
// internal ftoa variant for exponential floating-point type, contributed by Martijn Jasperse <m.jasperse@gmail.com>
static size_t _etoa(out_fct_type out, char *buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags)
{
	// check for NaN and special values
	if ((value != value) || (value > DBL_MAX) || (value < -DBL_MAX))
	{
		return _ftoa(out, buffer, idx, maxlen, value, prec, width, flags);
	}

	// determine the sign
	const bool negative = value < 0;
	if (negative)
	{
		value = -value;
	}

	// default precision
	if (!(flags & FLAGS_PRECISION))
	{
		prec = PRINTF_DEFAULT_FLOAT_PRECISION;
	}

	// determine the decimal exponent
	// based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
	union {
		uint64_t U;
		double F;
	} conv;

	conv.F = value;
	int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023;			 // effectively log2
	conv.U = (conv.U & ((1ULL << 52U) - 1U)) | (1023ULL << 52U); // drop the exponent so conv.F is now in [1,2)
	// now approximate log10 from the log2 integer part and an expansion of ln around 1.5
	int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 + (conv.F - 1.5) * 0.289529654602168);
	// now we want to compute 10^expval but we want to be sure it won't overflow
	exp2 = (int)(expval * 3.321928094887362 + 0.5);
	const double z = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
	const double z2 = z * z;
	conv.U = (uint64_t)(exp2 + 1023) << 52U;
	// compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
	conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
	// correct for rounding errors
	if (value < conv.F)
	{
		expval--;
		conv.F /= 10;
	}

	// the exponent format is "%+03d" and largest value is "307", so set aside 4-5 characters
	unsigned int minwidth = ((expval < 100) && (expval > -100)) ? 4U : 5U;

	// in "%g" mode, "prec" is the number of *significant figures* not decimals
	if (flags & FLAGS_ADAPT_EXP)
	{
		// do we want to fall-back to "%f" mode?
		if ((value >= 1e-4) && (value < 1e6))
		{
			if ((int)prec > expval)
			{
				prec = (unsigned)((int)prec - expval - 1);
			}
			else
			{
				prec = 0;
			}
			flags |= FLAGS_PRECISION; // make sure _ftoa respects precision
			// no characters in exponent
			minwidth = 0U;
			expval = 0;
		}
		else
		{
			// we use one sigfig for the whole part
			if ((prec > 0) && (flags & FLAGS_PRECISION))
			{
				--prec;
			}
		}
	}

	// will everything fit?
	unsigned int fwidth = width;
	if (width > minwidth)
	{
		// we didn't fall-back so subtract the characters required for the exponent
		fwidth -= minwidth;
	}
	else
	{
		// not enough characters, so go back to default sizing
		fwidth = 0U;
	}
	if ((flags & FLAGS_LEFT) && minwidth)
	{
		// if we're padding on the right, DON'T pad the floating part
		fwidth = 0U;
	}

	// rescale the float value
	if (expval)
	{
		value /= conv.F;
	}

	// output the floating part
	const size_t start_idx = idx;
	idx = _ftoa(out, buffer, idx, maxlen, negative ? -value : value, prec, fwidth, flags & ~FLAGS_ADAPT_EXP);

	// output the exponent part
	if (minwidth)
	{
		// output the exponential symbol
		out((flags & FLAGS_UPPERCASE) ? 'E' : 'e', buffer, idx++, maxlen);
		// output the exponent value
		idx = _ntoa_long(out, buffer, idx, maxlen, (expval < 0) ? -expval : expval, expval < 0, 10, 0, minwidth - 1, FLAGS_ZEROPAD | FLAGS_PLUS);
		// might need to right-pad spaces
		if (flags & FLAGS_LEFT)
		{
			while (idx - start_idx < width)
				out(' ', buffer, idx++, maxlen);
		}
	}
	return idx;
}
#endif // PRINTF_SUPPORT_EXPONENTIAL
#endif // PRINTF_SUPPORT_FLOAT

#define popFunc_integer(name) (name) = val->pack.value.integer;
#define popFunc_number(name) (name) = val->pack.value.number;
#define popFunc_string(name) (name) = melM_strDataFromObj(val->pack.obj);

#define melM_popParam(vm, params, idx, typeName, typeId, name)  \
	{                                                           \
		Value *val = melGetIndexArray((vm), (params), (idx)++); \
                                                                \
		if (val == NULL || val->type != typeId)                 \
		{                                                       \
			return 1;                                           \
		}                                                       \
                                                                \
		popFunc_##typeName(name)                                \
	}                                                           \
	do                                                          \
	{                                                           \
	} while (false)

#define melM_popBoxedParam(vm, params, idx, typeName, typeId, name) \
	melM_popParam(vm, params, idx, typeName, typeId, name)

#define melM_popBoxedIntParam(vm, params, idx, name) \
	melM_popBoxedParam(vm, params, idx, integer, MELON_TYPE_INTEGER, name)

#define melM_popBoxedNumParam(vm, params, idx, name) \
	melM_popBoxedParam(vm, params, idx, number, MELON_TYPE_NUMBER, name)

#define melM_popBoxedStrParam(vm, params, idx, name) \
	melM_popBoxedParam(vm, params, idx, string, MELON_TYPE_STRING, name)

static void _sfmt_out(char c, void* buffer, size_t idx, size_t maxlen)
{
	struct StrFormat* sf = (struct StrFormat*)buffer;
	melStringFmtWriteCharAt(sf, c, idx);
}

TRet melFormat(VM* vm, Value* outString, GCItem* format, GCItem* params)
{
	if (outString == NULL || format == NULL || params == NULL)
	{
		return 1;
	}

	if (format->type != MELON_TYPE_STRING || params->type != MELON_TYPE_ARRAY)
	{
		return 1;
	}

	size_t maxlen = SIZE_MAX;
	unsigned int flags, width, precision, n;
	size_t idx = 0U;

	out_fct_type out = _sfmt_out;

	const char *formatData = melM_strDataFromObj(format);
	TSize arrayIdx = 0;

	struct StrFormat sf;
	memset(&sf, 0, sizeof(struct StrFormat));
	char *buffer = (char*)&sf;

	while (*formatData)
	{
		// format specifier?  %[flags][width][.precision][length]
		if (*formatData != '%')
		{
			// no
			out(*formatData, buffer, idx++, maxlen);
			formatData++;
			continue;
		}
		else
		{
			// yes, evaluate it
			formatData++;
		}

		// evaluate flags
		flags = 0U;
		do
		{
			switch (*formatData)
			{
			case '0':
				flags |= FLAGS_ZEROPAD;
				formatData++;
				n = 1U;
				break;
			case '-':
				flags |= FLAGS_LEFT;
				formatData++;
				n = 1U;
				break;
			case '+':
				flags |= FLAGS_PLUS;
				formatData++;
				n = 1U;
				break;
			case ' ':
				flags |= FLAGS_SPACE;
				formatData++;
				n = 1U;
				break;
			case '#':
				flags |= FLAGS_HASH;
				formatData++;
				n = 1U;
				break;
			default:
				n = 0U;
				break;
			}
		} while (n);

		// evaluate width field
		width = 0U;
		if (_is_digit(*formatData))
		{
			width = _atoi(&formatData);
		}
		else if (*formatData == '*')
		{
			TInteger w;
			melM_popBoxedIntParam(vm, params, arrayIdx, w);

			if (w < 0)
			{
				flags |= FLAGS_LEFT; // reverse padding
				width = (unsigned int)-w;
			}
			else
			{
				width = (unsigned int)w;
			}

			formatData++;
		}

		// evaluate precision field
		precision = 0U;
		if (*formatData == '.')
		{
			flags |= FLAGS_PRECISION;
			formatData++;

			if (_is_digit(*formatData))
			{
				precision = _atoi(&formatData);
			}
			else if (*formatData == '*')
			{
				TInteger prec;
				melM_popBoxedIntParam(vm, params, arrayIdx, prec);

				precision = prec > 0 ? (unsigned int)prec : 0U;
				formatData++;
			}
		}

		// evaluate length field
		switch (*formatData)
		{
		case 'l':
			flags |= FLAGS_LONG;
			formatData++;
			if (*formatData == 'l')
			{
				flags |= FLAGS_LONG_LONG;
				formatData++;
			}
			break;
		case 'h':
			flags |= FLAGS_SHORT;
			formatData++;
			if (*formatData == 'h')
			{
				flags |= FLAGS_CHAR;
				formatData++;
			}
			break;
#if defined(PRINTF_SUPPORT_PTRDIFF_T)
		case 't':
			flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
			formatData++;
			break;
#endif
		case 'j':
			flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
			formatData++;
			break;
		case 'z':
			flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
			formatData++;
			break;
		default:
			break;
		}

		// evaluate specifier
		switch (*formatData)
		{
		case 'd':
		case 'i':
		case 'u':
		case 'x':
		case 'X':
		case 'o':
		case 'b':
		{
			// set the base
			unsigned int base;
			if (*formatData == 'x' || *formatData == 'X')
			{
				base = 16U;
			}
			else if (*formatData == 'o')
			{
				base = 8U;
			}
			else if (*formatData == 'b')
			{
				base = 2U;
			}
			else
			{
				base = 10U;
				flags &= ~FLAGS_HASH; // no hash for dec format
			}
			// uppercase
			if (*formatData == 'X')
			{
				flags |= FLAGS_UPPERCASE;
			}

			// no plus or space flag for u, x, X, o, b
			if ((*formatData != 'i') && (*formatData != 'd'))
			{
				flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
			}

			// ignore '0' flag when precision is given
			if (flags & FLAGS_PRECISION)
			{
				flags &= ~FLAGS_ZEROPAD;
			}

			// convert the integer
			if ((*formatData == 'i') || (*formatData == 'd'))
			{
				// signed
				TInteger value;
				melM_popBoxedIntParam(vm, params, arrayIdx, value);
				idx = _ntoa_long_long(out, buffer, idx, maxlen, (unsigned long long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
			}
			else
			{
				// unsigned
				TInteger value;
				melM_popBoxedIntParam(vm, params, arrayIdx, value);
				idx = _ntoa_long_long(out, buffer, idx, maxlen, (unsigned long long)value, false, base, precision, width, flags);
			}
			formatData++;
			break;
		}
#if defined(PRINTF_SUPPORT_FLOAT)
		case 'f':
		case 'F':
			{
				TNumber floatVal;

				if (*formatData == 'F')
					flags |= FLAGS_UPPERCASE;
				melM_popBoxedNumParam(vm, params, arrayIdx, floatVal);
				idx = _ftoa(out, buffer, idx, maxlen, floatVal, precision, width, flags);
				formatData++;
			}
			break;
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			{
				TNumber floatVal;

				if ((*formatData == 'g') || (*formatData == 'G'))
					flags |= FLAGS_ADAPT_EXP;
				if ((*formatData == 'E') || (*formatData == 'G'))
					flags |= FLAGS_UPPERCASE;
				melM_popBoxedNumParam(vm, params, arrayIdx, floatVal);
				idx = _etoa(out, buffer, idx, maxlen, floatVal, precision, width, flags);
				formatData++;
			}
			break;
#endif // PRINTF_SUPPORT_EXPONENTIAL
#endif // PRINTF_SUPPORT_FLOAT
		case 'c':
		case 's':
		{
			const char *p;
			melM_popBoxedStrParam(vm, params, arrayIdx, p);
			unsigned int l = _strnlen_s(p, precision ? precision : (size_t)-1);
			// pre padding
			if (flags & FLAGS_PRECISION)
			{
				l = (l < precision ? l : precision);
			}
			if (!(flags & FLAGS_LEFT))
			{
				while (l++ < width)
				{
					out(' ', buffer, idx++, maxlen);
				}
			}
			// string output
			while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--))
			{
				out(*(p++), buffer, idx++, maxlen);
			}
			// post padding
			if (flags & FLAGS_LEFT)
			{
				while (l++ < width)
				{
					out(' ', buffer, idx++, maxlen);
				}
			}
			formatData++;
			break;
		}

		case 'p':
		{
			width = sizeof(void *) * 2U;
			flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
			const bool is_ll = sizeof(uintptr_t) == sizeof(long long);

			TPtr ptr = 0;

			Value *val = melGetIndexArray(vm, params, arrayIdx);

			if (val == NULL)
			{
				return 1;
			}

			if (melM_isGCItem(val))
			{
				ptr = (TPtr)val->pack.obj;
			}

			if (is_ll)
			{
				idx = _ntoa_long_long(out, buffer, idx, maxlen, (uintptr_t)ptr, false, 16U, precision, width, flags);
			}
			else
			{
				idx = _ntoa_long(out, buffer, idx, maxlen, (unsigned long)((uintptr_t)ptr), false, 16U, precision, width, flags);
			}

			formatData++;
			break;
		}

		case '%':
			out('%', buffer, idx++, maxlen);
			formatData++;
			break;

		default:
			out(*formatData, buffer, idx++, maxlen);
			format++;
			break;
		}
	}

	// termination
	out((char)0, buffer, idx < maxlen ? idx : maxlen - 1U, maxlen);

	outString->pack.obj = melNewString(vm, sf.buffer, sf.used);
	outString->type = outString->pack.obj->type;

	return 0;
}
