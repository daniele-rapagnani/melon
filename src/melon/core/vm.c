#include "melon/core/config.h"
#include "melon/core/vm.h"
#include "melon/core/tstring.h"
#include "melon/core/object.h"
#include "melon/core/array.h"
#include "melon/core/symbol.h"
#include "melon/core/range.h"
#include "melon/core/program.h"
#include "melon/core/deserializer.h"
#include "melon/core/opcodes.h"
#include "melon/core/stack.h"
#include "melon/core/utils.h"
#include "melon/core/native_iterator.h"
#include "melon/core/closure.h"
#include "melon/modules/modules.h"
#include "melon/modules/system_modules.h"
#include "melon/modules/string/string_module.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define melM_getInst(vm) ((vm)->function->code[(vm)->pc])

#define melM_isNumericType(v) \
    ((v) == MELON_TYPE_INTEGER || (v) == MELON_TYPE_NUMBER)

#define melM_popType(vm, typeName, varName) \
    (melPopStackType((vm), typeName))->pack.value.varName

#define melM_popInt(vm) melM_popType(vm, MELON_TYPE_INTEGER, integer)
#define melM_popNum(vm) melM_popType(vm, MELON_TYPE_NUMBER, number)

#define melM_binOpDef(name, operator, typeName, varName)                 \
    static void op##name##typeName(VM * vm)                            \
    {                                                                    \
        vm->stack.top--;                                                 \
        vm->stack.stack[vm->stack.top - 1].pack.value.varName =          \
            (vm->stack.stack[vm->stack.top - 1].pack.value.varName)      \
            operator (vm->stack.stack[vm->stack.top].pack.value.varName);\
    }

#define melM_integerBinOpDef(name, operator) \
    melM_binOpDef(name, operator, Int, integer)

#define melM_numberBinOpDef(name, operator) \
    melM_binOpDef(name, operator, Num, number)

#define melM_numericBinOpDef(name, operator)                                                \
    melM_integerBinOpDef(name, operator)                                                    \
    melM_numberBinOpDef(name, operator)                                                     \
                                                                                            \
    static void op##name(VM *vm)                                                            \
    {                                                                                       \
        TType b = melPeekStackType(vm, 0);                                                  \
        TType a = melPeekStackType(vm, 1);                                                  \
                                                                                            \
        if (a == MELON_TYPE_OBJECT)                                                         \
        {                                                                                   \
            if (melPerformCustomOpVM(vm, MELON_OBJSYM_##name, 2) != 0)                      \
            {                                                                               \
                melM_fatal(                                                                 \
                    vm,                                                                     \
                    "Performing " #name " on an object with no custom " #name " operator"); \
            }                                                                               \
            return;                                                                         \
        }                                                                                   \
                                                                                            \
        if (!melM_isNumericType(a) || !melM_isNumericType(b))                               \
        {                                                                                   \
            melM_fatal(                                                                     \
                vm,                                                                         \
                "Performing " #name " on unsupported types '%s' and '%s'",                  \
                MELON_TYPES_NAMES[a],                                                       \
                MELON_TYPES_NAMES[b]);                                                      \
            return;                                                                         \
        }                                                                                   \
                                                                                            \
        if (a == MELON_TYPE_INTEGER && b == MELON_TYPE_INTEGER)                             \
        {                                                                                   \
            op##name##Int(vm);                                                              \
            return;                                                                         \
        }                                                                                   \
                                                                                            \
        if (a != b)                                                                         \
        {                                                                                   \
            Value *vb = melM_stackOffset(&vm->stack, 0);                                    \
            Value *va = melM_stackOffset(&vm->stack, 1);                                    \
                                                                                            \
            if (a == MELON_TYPE_INTEGER)                                                    \
            {                                                                               \
                va->type = MELON_TYPE_NUMBER;                                               \
                va->pack.value.number = va->pack.value.integer;                             \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                vb->type = MELON_TYPE_NUMBER;                                               \
                vb->pack.value.number = vb->pack.value.integer;                             \
            }                                                                               \
        }                                                                                   \
                                                                                            \
        op##name##Num(vm);                                                                  \
    }

#define melM_unaryOpDef(name, operator, typeName, varName)                    \
    static void op##name##typeName(VM *vm)                                    \
    {                                                                         \
        vm->stack.stack[vm->stack.top - 1].pack.value.varName =               \
            operator (vm->stack.stack[vm->stack.top - 1].pack.value.varName); \
    }

#define melM_integerUnaryOpDef(name, operator) \
    melM_unaryOpDef(name, operator, Int, integer)

#define melM_numberUnaryOpDef(name, operator) \
	melM_unaryOpDef(name, operator, Num, number)

#define melM_numericUnaryOpDef(name, operator)                                                    \
    melM_integerUnaryOpDef(name, operator)                                                        \
    melM_numberUnaryOpDef(name, operator)                                                         \
                                                                                                  \
	static void op##name(VM *vm)                                                                  \
	{                                                                                             \
		TType a = melPeekStackType(vm, 0);                                                        \
                                                                                                  \
		if (a == MELON_TYPE_OBJECT)                                                               \
		{                                                                                         \
			if (melPerformCustomOpVM(vm, MELON_OBJSYM_##name, 1) != 0)                            \
			{                                                                                     \
				melM_fatal(                                                                       \
					vm,                                                                           \
					"Performing unary " #name " on an object with no custom " #name " operator"); \
			}                                                                                     \
			return;                                                                               \
		}                                                                                         \
                                                                                                  \
		if (a != MELON_TYPE_INTEGER && a != MELON_TYPE_NUMBER)                                    \
		{                                                                                         \
			melM_fatal(                                                                           \
				vm,                                                                               \
				"Performing " #name " on unsupported type '%s'",                                  \
				MELON_TYPES_NAMES[a]);                                                            \
			return;                                                                               \
		}                                                                                         \
                                                                                                  \
		if (a == MELON_TYPE_INTEGER)                                                              \
		{                                                                                         \
			op##name##Int(vm);                                                                    \
			return;                                                                               \
		}                                                                                         \
                                                                                                  \
		op##name##Num(vm);                                                                        \
	}

#define melM_logicBinOpDef(name, operator)                                             \
    static void opLogic##name(VM *vm)                                                  \
    {                                                                                  \
        StackEntry *b = melPopStackType(vm, MELON_TYPE_BOOL);                          \
        StackEntry *a = melPopStackType(vm, MELON_TYPE_BOOL);                          \
                                                                                       \
        Value val;                                                                     \
        val.type = MELON_TYPE_BOOL;                                                    \
        val.pack.value.boolean = a->pack.value.boolean operator b->pack.value.boolean; \
                                                                                       \
        melM_stackPushRaw(&vm->stack, &val);                                                \
    }

#define melM_logicUnaryOpDef(name, operator)                     \
    static void opLogic##name(VM *vm)                            \
    {                                                            \
        StackEntry *a = melPopStackType(vm, MELON_TYPE_BOOL);    \
                                                                 \
        Value val;                                               \
        val.type = MELON_TYPE_BOOL;                              \
        val.pack.value.boolean = operator a->pack.value.boolean; \
                                                                 \
        melM_stackPushRaw(&vm->stack, &val);                          \
    }

#ifndef NDEBUG
#define melM_check(x) assert(x == 0)
#else
#define melM_check(x) x
#endif

const TByte MAGIC_BYTES[] = {
    0x4d, 0x45, 0x4c, 0x4f, 0x4e
};

const TInteger INT_CHECK_VALUE = 0x99AABBCCDDEEFF;
const TNumber  NUM_CHECK_VALUE = -2045.123;

const char* MELON_TYPES_NAMES[] = {
    "none",
    "integer",
    "number",
    "bool",
    "null",
    "nativeptr",
    "smallrange",
    "nativeit",
    "string",
    "object",
    "closure",
    "program",
    "array",
    "symbol",
    "range",
    "function"
};

static Value nullValue;
static Value noneValue;

static Value nativeIteratorValueSymbol;
static Value nativeIteratorIdxSymbol;
static Value iteratorValueKey;
static Value iteratorDoneKey;

#define melM_vstackPushPop(vm, val, popCount) \
    melM_stackPopCount(&vm->stack, popCount); \
    melM_stackPush(&vm->stack, val)

#define melM_vstackPushPopRaw(vm, val, popCount) \
    melM_stackPopCount(&vm->stack, popCount); \
    melM_stackPushRaw(&vm->stack, val)

static Value* melPopStackType(VM* vm, TType type);
static TType melPeekStackType(VM* vm, StackSize offset);
static Value* melPeekStackOfType(VM* vm, TType type, StackSize offset);
static Value* getConstant(VM* vm, TConstantID k);

static TRet melNewCallFrame(VM* vm, TUint16 expRet);
static TSize melGetSlotToStack(VM* vm, CallFrame* cf, TSize slot, TBool ignoreMethod);

static TRet melStepOutOfFunction(VM* vm);
static TRet melReturnFunction(VM* vm, TUint32 valuesCount);

static Upvalue* melGetOrAddUpvalueVM(VM* vm, Value* value);
static TRet melCloseUpvaluesVM(VM* vm, Value* upToValue);

static Value* getValueFromGlobal(VM* vm, Value* key);
static TByte melPushIndexedIterator(VM* vm, TType type, NativeIteratorNext nextFunc);


static TRet melRangeNextNativeFunc(VM* vm, NativeIterator* ni);
static TRet melArrayNextNativeFunc(VM* vm, NativeIterator* ni);
static TRet melStringNextNativeFunc(VM* vm, NativeIterator* ni);
static TRet melObjectNextNativeFunc(VM* vm, NativeIterator* ni);

/******/

melM_numericBinOpDef(ADD, +);
melM_numericBinOpDef(SUB, -);
melM_numericBinOpDef(MUL, *);
melM_numericBinOpDef(DIV, /);
melM_numericUnaryOpDef(NEG, -);

melM_integerBinOpDef(BitLeftShift, <<);
melM_integerBinOpDef(BitRightShift, >>);
melM_integerBinOpDef(BitAnd, &);
melM_integerBinOpDef(BitOr, |);
melM_integerBinOpDef(BitXor, ^);
melM_integerBinOpDef(Mod, %);
melM_integerUnaryOpDef(BitNot, ~);

melM_logicBinOpDef(Or, ||);
melM_logicBinOpDef(And, &&);
melM_logicUnaryOpDef(Not, !);

static void opPow(VM* vm)
{
    Value* va = melM_stackOffset(&vm->stack, 1);
    Value* vb = melM_stackOffset(&vm->stack, 0);
    TType a = va->type;
    TType b = vb->type;

    if (a == MELON_TYPE_OBJECT)
    {
        if (melPerformCustomOpVM(vm, MELON_OBJSYM_POW, 2) != 0)
        {
            melM_fatal(vm, "Performing power on an object with no custom operator");
        }

        return;
    }

    if ((a != MELON_TYPE_INTEGER 
        && b != MELON_TYPE_INTEGER 
        && a != MELON_TYPE_NUMBER 
        && b != MELON_TYPE_NUMBER) 
        || a != b
    )
    {
        melM_fatal(
            vm,
            "Performing power operator on unsupported types '%s' and '%s'",
            MELON_TYPES_NAMES[a],
            MELON_TYPES_NAMES[b]);
        return;
    }

    Value result;
    result.type = a;

    if (a == MELON_TYPE_INTEGER)
    {
        result.pack.value.integer = pow(va->pack.value.integer, vb->pack.value.integer);
    }
    else
    {
        result.pack.value.number = pow(va->pack.value.number, vb->pack.value.number);
    }
    
    melM_vstackPushPop(vm, &result, 2);
}

static void opCoalesce(VM* vm)
{
    Value* va = melM_stackOffset(&vm->stack, 1);
    Value* vb = melM_stackOffset(&vm->stack, 0);

    TType a = va->type;
    TType b = vb->type;

    if (a != MELON_TYPE_NULL || (a == MELON_TYPE_NULL && a == b))
    {
        // Leave a on the stack
        melM_stackPop(&vm->stack);
        return;
    }
    
    *va = *vb;
    melM_stackPop(&vm->stack);
}

static void opPushK(VM* vm, Instruction* i)
{
    TConstantID kId = melM_op_geta25(i->inst);
    Value* val = getConstant(vm, kId);

    if (val == NULL)
    {
        melM_fatal(vm, "Can't load constant with id %lu", kId);
        return;
    }

    melM_stackPush(&vm->stack, val);
}

static void opPushI(VM* vm, Instruction* i)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* val = melM_stackAllocRaw(&vm->stack);
    val->type = MELON_TYPE_INTEGER;
    val->pack.value.integer = melM_op_geta25s(i->inst);
}

static void opConcat(VM* vm)
{
    Value* a = melM_stackOffset(&vm->stack, 1);
    Value* b = melM_stackOffset(&vm->stack, 0);

    if (a->type != b->type)
    {
        melM_fatal(
            vm, 
            "Can't concat a '%s' with an instance of '%s'", 
            MELON_TYPES_NAMES[a->type], 
            MELON_TYPES_NAMES[b->type]
        );

        return;
    }

    Value val;

    switch(a->type)
    {
        case MELON_TYPE_STRING:
            {
                val.type = MELON_TYPE_STRING;
                val.pack.obj = melNewStringFromStrings(vm, a->pack.obj, b->pack.obj);;
            }
            break;

        case MELON_TYPE_ARRAY:
            {
                val.type = MELON_TYPE_ARRAY;
                val.pack.obj = melNewArrayFromArrays(vm, a->pack.obj, b->pack.obj);
            }
            break;

        case MELON_TYPE_INTEGER:
        {
            if (melM_rangeIsSmall(a->pack.value.integer, b->pack.value.integer))
            {
                val.type = MELON_TYPE_SMALLRANGE;
                val.pack.value.integer = melM_rangeSmallGet(a->pack.value.integer, b->pack.value.integer);
            }
            else
            {
                val.type = MELON_TYPE_RANGE;
                val.pack.obj = melNewRange(vm, a->pack.value.integer, b->pack.value.integer);
            }
            break;
        }
    }

    melM_vstackPushPop(vm, &val, 2);
}

static void opJmp(VM* vm, Instruction* i, TUint64* pc)
{
    TVMInstSK offset = melM_op_geta25s(i->inst);
    *pc += offset;
}

static void opTestTrue(VM* vm, Instruction* i, TUint64* pc)
{
    TVMInstK popValue = melM_op_geta25(i->inst);
    Value* v = melPeekStackOfType(vm, MELON_TYPE_BOOL, 0);
    *pc += v->pack.value.boolean;
    assert(popValue < 2);
    vm->stack.top -= popValue;
}

static void opTestFalse(VM* vm, Instruction* i, TUint64* pc)
{
    TVMInstK popValue = melM_op_geta25(i->inst);
    Value* v = melPeekStackOfType(vm, MELON_TYPE_BOOL, 0);
    (*pc)++;
    *pc -= v->pack.value.boolean;
    assert(popValue < 2);
    vm->stack.top -= popValue;
}

static void opTestNull(VM* vm)
{
    Value* v = melM_stackOffset(&vm->stack, 0);

    if (v->type == MELON_TYPE_NULL)
    {
        melGetTopCallFrameVM(vm)->pc++;
        melM_stackPop(&vm->stack);
    }
}

static void opEq(VM* vm)
{
	// TODO: Looks a lot like the cmp (obviously).
	//       Does having two different instructions still make sense?

    StackEntry* a = melM_stackOffset(&vm->stack, 1);
    StackEntry* b = melM_stackOffset(&vm->stack, 0);

	TBool compared = 0;
	TBool consumed = 0;

    Value val;
    val.type = MELON_TYPE_BOOL;

    static TRet r = 1;

	if (a->type == MELON_TYPE_OBJECT)
    {
        if (melPerformCustomOpVM(vm, MELON_OBJSYM_CMP, 2) == 0)
        {
            Value result = *melM_stackPop(&vm->stack);

            if (result.type != MELON_TYPE_INTEGER)
            {
                melM_fatal(vm, "The custom equality operator must return an integer");
                return;
            }

            r = result.pack.value.integer;
			compared = 1;
			consumed = 1;
        }
    }

	if (!compared)
	{
		if (melCmpValues(vm, a, b, &r) != 0)
		{
			melM_fatal(vm, "Can't compare values of types: %d and %d", a->type, b->type);
			return;
		}
	}

    val.pack.value.boolean = r == 0 ? 1 : 0;

    melM_vstackPushPop(vm, &val, consumed ? 0 : 2);
}

static void opCmp(VM* vm, TVMOpcode op)
{
    StackEntry* a = melM_stackOffset(&vm->stack, 1);
    StackEntry* b = melM_stackOffset(&vm->stack, 0);

	TBool compared = 0;
	TBool consumed = 0;

    Value val;
    val.type = MELON_TYPE_BOOL;
	static TRet r = 1;

	if (a->type == MELON_TYPE_OBJECT)
    {
        if (melPerformCustomOpVM(vm, MELON_OBJSYM_CMP, 2) == 0)
        {
            Value result = *melM_stackPop(&vm->stack);

            if (result.type != MELON_TYPE_INTEGER)
            {
                melM_fatal(vm, "The custom equality operator must return an integer");
                return;
            }

            r = result.pack.value.integer;
			compared = 1;
			consumed = 1;
        }
    }
    
    if (!compared)
	{
		if (melCmpValues(vm, a, b, &r) != 0)
		{
			melM_fatal(vm, "Can't compare values of types: %d and %d", a->type, b->type);
			return;
		}
	}

    switch(op)
    {
        case MELON_OP_LT:
            val.pack.value.boolean = r < 0 ? 1 : 0;
            break;

        case MELON_OP_LTE:
            val.pack.value.boolean = r <= 0 ? 1 : 0;
            break;

        case MELON_OP_GT:
            val.pack.value.boolean = r > 0 ? 1 : 0;
            break;

        case MELON_OP_GTE:
            val.pack.value.boolean = r >= 0 ? 1 : 0;
            break;

        default: 
            break;
    }

	melM_vstackPushPop(vm, &val, consumed ? 0 : 2);
}

static void opNewObj(VM* vm)
{
    Value val;
    val.type = MELON_TYPE_OBJECT;
    val.pack.obj = melNewObject(vm);
    melM_stackPush(&vm->stack, &val);
}

static void opSetObj(VM* vm)
{
    StackEntry* valueSe = melM_stackOffset(&vm->stack, 0);
    Value* field = melM_stackOffset(&vm->stack, 1);
    Value* obj = melPeekStackOfType(vm, MELON_TYPE_OBJECT, 2);

    if (
        !melM_objectHasCustomSymbol(obj->pack.obj, MELON_OBJSYM_SETPROPERTY)
        || melPerformCustomOpVM(vm, MELON_OBJSYM_SETPROPERTY, 3) != 0
    )
    {
        melSetValueObject(vm, obj->pack.obj, field, valueSe);
        melM_stackPopCount(&vm->stack, 3);
    }
}

static void opGetObj(VM* vm, Instruction* i)
{
    TBool repushObj = melM_op_geta25(i->inst) != 0;
    Value* field = melM_stackOffset(&vm->stack, 0);
    Value* obj = melPeekStackOfType(vm, MELON_TYPE_OBJECT, 1);

    if (
        !melM_objectHasCustomSymbol(obj->pack.obj, MELON_OBJSYM_PROPERTY)
        || melPerformCustomOpVM(vm, MELON_OBJSYM_PROPERTY, 2) != 0
    )
    {
        Value* value = melResolveValueObject(vm, obj->pack.obj, field);

        if (value == NULL)
        {
            melM_stackPopCount(&vm->stack, 2);
            melM_vstackPushNull(&vm->stack);
            return;
        }

        if (repushObj)
        {
            // Copy the object because we are going to modify the stack
            // and thus invalidate the 'obj' pointer
            Value objCpy = *obj;

            melM_vstackPushPop(vm, value, 2);
            melM_stackPushRaw(&vm->stack, &objCpy);
        }
        else
        {
            melM_vstackPushPopRaw(vm, value, 2);
        }
    }
}

static void opBless(VM* vm)
{
    Value* blessee = melPeekStackOfType(vm, MELON_TYPE_OBJECT, 1);
    Value* blessing = melPeekStackOfType(vm, MELON_TYPE_OBJECT, 0);

    melSetPrototypeObject(vm, blessee->pack.obj, blessing->pack.obj);

    melM_stackPopCount(&vm->stack, 1);
}

static void opPushBlessing(VM* vm)
{
    Value* obj = melPeekStackOfType(vm, MELON_TYPE_OBJECT, 0);

    GCItem* proto = melGetPrototypeObject(vm, obj->pack.obj);

    melM_stackPopCount(&vm->stack, 1);

    if (proto == NULL)
    {
        melM_vstackPushNull(&vm->stack);
    }
    else
    {
        melM_vstackPushGCItem(&vm->stack, proto);
    }
}

static void opSetGlobal(VM* vm)
{
    StackEntry* valueSe = melM_stackOffset(&vm->stack, 0);
    Value* field = melPeekStackOfType(vm, MELON_TYPE_STRING, 1);
    melSetValueObject(vm, vm->global, field, valueSe);

    melM_stackPopCount(&vm->stack, 2);
}

static void opGetGlobal(VM* vm)
{
    Value* field = melPeekStackOfType(vm, MELON_TYPE_STRING, 0);
    Value* value = melGetValueObject(vm, vm->global, field);

    if (value == NULL)
    {
        melM_stackPop(&vm->stack);
        melM_vstackPushNull(&vm->stack);

        return;
    }

    melM_vstackPushPop(vm, value, 1);
}

static void opNewSymbol(VM* vm, Instruction* i)
{
    TUint32 withString = melM_op_geta25(i->inst);

    Value* label = NULL;

    if (withString)
    {
        label = melPeekStackOfType(vm, MELON_TYPE_STRING, 0);
    }

    GCItem* symbol = melNewSymbol(vm, label != NULL ? label->pack.obj : NULL);

    if (label != NULL)
    {
        melM_stackPop(&vm->stack);
    }

    melM_vstackPushGCItem(&vm->stack, symbol);
}

static void opNewArray(VM* vm)
{
    Value val;
    val.type = MELON_TYPE_ARRAY;
    val.pack.obj = melNewArray(vm);
    melM_stackPush(&vm->stack, &val);
}

static void opGetArray(VM* vm)
{
    Value* index = melM_stackOffset(&vm->stack, 0);
    Value* arr = melM_stackOffset(&vm->stack, 1);

    switch(arr->type)
    {
        case MELON_TYPE_ARRAY:
            {
                if (index->type != MELON_TYPE_INTEGER)
                {
                    melM_fatal(vm, "Can't use instance of type '%s' as array index.", MELON_TYPES_NAMES[index->type]);
                    return;
                }

                Value* value = melGetIndexArray(vm, arr->pack.obj, index->pack.value.integer);

                if (value == NULL)
                {
                    melM_fatal(vm, "Invalid array index to get: %d", index->pack.value.integer);
                    return;
                }

                melM_vstackPushPop(vm, value, 2);
            }
            break;

        case MELON_TYPE_STRING:
            {
                if (index->type != MELON_TYPE_INTEGER)
                {
                    melM_fatal(vm, "Can't use instance of type '%s' as string index.", MELON_TYPES_NAMES[index->type]);
                    return;
                }

                String* str = melM_strFromObj(arr->pack.obj);
                const char* strData = melM_strDataFromObj(arr->pack.obj);

                if (index->pack.value.integer >= str->len)
                {
                    melM_fatal(
                        vm, 
                        "Invalid string index to get: %d, string is %d chars long", 
                        index->pack.value.integer, 
                        str->len
                    );

                    return;
                }

                Value strVal;
                strVal.type = MELON_TYPE_STRING;
                strVal.pack.obj = melNewString(vm, &strData[index->pack.value.integer], 1);

                melM_vstackPushPop(vm, &strVal, 2);
            }
            break;

        case MELON_TYPE_OBJECT:
            {
				if (melPerformCustomOpVM(vm, MELON_OBJSYM_INDEX, 2) != 0)
				{
					Value* value = melResolveValueObject(vm, arr->pack.obj, index);

					if (value != NULL)
					{
						melM_vstackPushPop(vm, value, 2);
					}
					else
					{
						melM_stackPopCount(&vm->stack, 2);
                        melM_vstackPushNull(&vm->stack);
					}
				}
            }
            break;

        default:
            melM_fatal(vm, "Can't use array access operator on instance of type '%s'", MELON_TYPES_NAMES[arr->type]);
            return;
    }
}

static void opGetIArray(VM* vm, Instruction* i)
{
    TUint32 index = melM_op_geta25(i->inst);
    Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 0);

    Value* value = melGetIndexArray(vm, arr->pack.obj, index);

    if (value == NULL)
    {
        melM_fatal(vm, "Invalid array index to get: %d", index);
        return;
    }

    melM_vstackPushPop(vm, value, 1);
}

static void opSetArray(VM* vm)
{
    StackEntry* valueSe = melM_stackOffset(&vm->stack, 0);
    Value* index = melM_stackOffset(&vm->stack, 1);
    Value* arr = melM_stackOffset(&vm->stack, 2);

    switch(arr->type)
    {
        case MELON_TYPE_ARRAY:
            if (index->type != MELON_TYPE_INTEGER)
            {
                melM_fatal(vm, "Can't use instance of type '%s' as array index.", MELON_TYPES_NAMES[index->type]);
                return;
            }

            if (melSetIndexArray(vm, arr->pack.obj, index->pack.value.integer, valueSe) != 0)
            {
                melM_fatal(vm, "Invalid array index to set: %d", index->pack.value.integer);
                return;
            }
            break;

        case MELON_TYPE_OBJECT:
			{
				if (melPerformCustomOpVM(vm, MELON_OBJSYM_SETINDEX, 3) == 0)
				{
					// No need to pop
					return;
				}

				melSetValueObject(vm, arr->pack.obj, index, valueSe);

				break;
			}

        default:
            melM_fatal(vm, "Can't use indexed assignment on instance of type '%s'", MELON_TYPES_NAMES[arr->type]);
            return;
    }

    melM_stackPopCount(&vm->stack, 3);
}

static void opSetIArray(VM* vm, Instruction* i)
{
    StackEntry* valueSe = melM_stackOffset(&vm->stack, 0);
    TInt32 index = melM_op_geta25(i->inst);
    Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 1);

    if (melSetIndexArray(vm, arr->pack.obj, index, valueSe) != 0)
    {
        melM_fatal(vm, "Invalid array index to set: %d", index);
        return;
    }

    melM_stackPopCount(&vm->stack, 2);
}

static void opPushArray(VM* vm)
{
    StackEntry* valueSe = melM_stackOffset(&vm->stack, 0);
    Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 1);

    melPushArray(vm, arr->pack.obj, valueSe);

    melM_stackPopCount(&vm->stack, 2);
}

static const TInt32 SLICE_COPY = 0;
static const TInt32 SLICE_START = 1;
static const TInt32 SLICE_END = 2;
static const TInt32 SLICE_START_END = 3;

static void opSlice(VM* vm, Instruction* i)
{
    TInt32 index = melM_op_geta25(i->inst);
    
    switch(index)
    {
        case SLICE_COPY:
            {
                Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 0);
                GCItem* newArr = melNewArrayFromArray(vm, arr->pack.obj, NULL, NULL);
                assert(newArr != NULL);

                melM_stackPopCount(&vm->stack, 1);
                melM_vstackPushGCItem(&vm->stack, newArr);
            }
            break;

        case SLICE_START:
            {
                Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 1);
                Value* start = melPeekStackOfType(vm, MELON_TYPE_INTEGER, 0);

                GCItem* newArr = melNewArrayFromArray(vm, arr->pack.obj, &start->pack.value.integer, NULL);
                assert(newArr != NULL);

                melM_stackPopCount(&vm->stack, 2);
                melM_vstackPushGCItem(&vm->stack, newArr);
            }
            break;

        case SLICE_END:
            {
                Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 1);
                Value* end = melPeekStackOfType(vm, MELON_TYPE_INTEGER, 0);

                GCItem* newArr = melNewArrayFromArray(vm, arr->pack.obj, NULL, &end->pack.value.integer);
                assert(newArr != NULL);

                melM_stackPopCount(&vm->stack, 2);
                melM_vstackPushGCItem(&vm->stack, newArr);
            }
            break;

        case SLICE_START_END:
            {
                Value* arr = melPeekStackOfType(vm, MELON_TYPE_ARRAY, 2);
                Value* start = melPeekStackOfType(vm, MELON_TYPE_INTEGER, 1);
                Value* end = melPeekStackOfType(vm, MELON_TYPE_INTEGER, 0);

                GCItem* newArr = melNewArrayFromArray(vm, arr->pack.obj, &start->pack.value.integer, &end->pack.value.integer);
                assert(newArr != NULL);

                melM_stackPopCount(&vm->stack, 3);
                melM_vstackPushGCItem(&vm->stack, newArr);
            }
            break;

        default:
            melM_fatal(vm, "Invalid slice operation");
            break;
    }
}

static void opSizeArray(VM* vm)
{
    Value* arr = melM_stackOffset(&vm->stack, 0);

    switch(arr->type)
    {
        case MELON_TYPE_ARRAY:
        {
            Array* arrObj = melM_arrayFromObj(arr->pack.obj);

            Value val;
            val.type = MELON_TYPE_INTEGER;
            val.pack.value.integer = arrObj->count;
            
            melM_vstackPushPop(vm, &val, 1);
        }
        break;

        case MELON_TYPE_STRING:
        {
            String* strObj = melM_strFromObj(arr->pack.obj);

            Value val;
            val.type = MELON_TYPE_INTEGER;
            val.pack.value.integer = strObj->len;
            
            melM_vstackPushPop(vm, &val, 1);
        }
        break;

        case MELON_TYPE_OBJECT:
        {	
			if (melPerformCustomOpVM(vm, MELON_OBJSYM_SIZEARR, 1) != 0)
			{
				Object* objObj = melM_objectFromObj(arr->pack.obj);

				Value val;
				val.type = MELON_TYPE_INTEGER;
				val.pack.value.integer = objObj->count;
				
				melM_vstackPushPop(vm, &val, 1);
			}
        }
        break;

        default:
            melM_fatal(vm, "Can't get size of instance of type '%s'", MELON_TYPES_NAMES[arr->type]);
            return;
    }
}

static void opLoad(VM* vm, Instruction* i, CallFrame* cf)
{
    TInt32 slot = melM_op_geta25(i->inst);
    Value* val = melGetLocalVM(vm, cf, slot, 0);
    assert(val != NULL);

    melM_stackPush(&vm->stack, val);
}

static void opPushThis(VM* vm, CallFrame* cf)
{
    Value* val = melGetLocalVM(vm, cf, 0, 1);
    assert(val != NULL);

    melM_stackPush(&vm->stack, val);
}

static void opStore(VM* vm, Instruction* i, CallFrame* cf)
{
    TInt32 slot = melM_op_geta25(i->inst);

    StackEntry* se = melM_stackOffset(&vm->stack, 0);
    melSetLocalVM(vm, cf, slot, se, 0);
    melM_stackPop(&vm->stack);
}

static void opStoreTopCL(VM* vm, Instruction* i, CallFrame* cf)
{
    TInt32 slot = melM_op_geta25(i->inst);
    TSize stackPos = melGetSlotToStack(vm, cf, slot, 0);
    Value* val = melM_stackGet(&vm->stack, stackPos);
    val->type = cf->closure->type;
    val->pack.obj = cf->closure;
}

static void opCall(VM* vm, Instruction* i, TRet tail)
{
    TUint16 callArgs = melM_op_geta13(i->inst);
    TUint16 expectedRes = melM_op_getb12(i->inst);
    melCallClosureVM(vm, callArgs, tail, expectedRes);    
}

static void opReturn(VM* vm, Instruction* i)
{
    TUint32 valuesCount = melM_op_geta25(i->inst);
    melReturnFunction(vm, valuesCount);
}

static void opNewClosure(VM* vm, Instruction* i)
{
    TUint32 protoNum = melM_op_geta25(i->inst);

    CallFrame* cf = melGetTopCallFrameVM(vm);
    GCItem* fnObj = NULL;

    if (protoNum == 0)
    {
        Closure* cl = melM_closureFromObj(cf->closure);
        fnObj = cl->fn;
    }
    else
    {        
        assert(protoNum - 1 < cf->function->funcPrototype.count);
        fnObj = cf->function->funcPrototype.prototypes[protoNum - 1];
    }

    GCItem* item = melNewClosure(vm, fnObj);
    Value val;
    val.type = MELON_TYPE_CLOSURE;
    val.pack.obj = item;

    melM_stackPush(&vm->stack, &val);

    Upvalue** upvalues = melM_closureUpvaluesFromObj(item);
    Upvalue** parentUpvalues = cf->closure != NULL ? melM_closureUpvaluesFromObj(cf->closure) : NULL;

    Function* fn = melM_functionFromObj(fnObj);

    for (TSize i = 0; i < fn->upvaluesInfos.count; i++)
    {
        UpvalueInfo* ui = &fn->upvaluesInfos.infos[i];

        if (ui->instack != 0)
        {
            upvalues[i] = melGetOrAddUpvalueVM(vm, melGetLocalVM(vm, cf, ui->idx, 0));
        }
        else
        {
            assert(parentUpvalues != NULL);
            assert(ui->idx < cf->function->upvaluesInfos.count);
            upvalues[i] = parentUpvalues[ui->idx];
        }

        upvalues[i]->refCount++;
    }
}

static void opPushUpval(VM* vm, Instruction* i)
{
    TUint32 upvalNum = melM_op_geta25(i->inst);
    CallFrame* cf = melGetTopCallFrameVM(vm);
 
    if (cf->closure == NULL)
    {
        melM_fatal(vm, "Trying to push upvalue %d of main on the stack.", upvalNum);
    }
 
    if (upvalNum >= cf->function->upvaluesInfos.count)
    {
        melM_fatal(
            vm, 
            "Trying to push invalid upvalue %d, function has %d upvalues", 
            upvalNum, 
            cf->function->upvaluesInfos.count
        );
    }

    Upvalue** upvalues = melM_closureUpvaluesFromObj(cf->closure);
    melM_stackPush(&vm->stack, upvalues[upvalNum]->value);
}

static void opStoreUpval(VM* vm, Instruction* i)
{
    TUint32 upvalNum = melM_op_geta25(i->inst);
    CallFrame* cf = melGetTopCallFrameVM(vm);
 
    if (cf->closure == NULL)
    {
        melM_fatal(vm, "Trying to store in upvalue %d of main on the stack.", upvalNum);
    }
 
    if (upvalNum >= cf->function->upvaluesInfos.count)
    {
        melM_fatal(
            vm, 
            "Trying to store in invalid upvalue %d, function has %d upvalues", 
            upvalNum, 
            cf->function->upvaluesInfos.count
        );
    }

    StackEntry* se = melM_stackOffset(&vm->stack, 0);
    Upvalue** upvalues = melM_closureUpvaluesFromObj(cf->closure);
    *(upvalues[upvalNum]->value) = *se;

    melM_stackPop(&vm->stack);
}

static void opPush(VM* vm, Instruction* i)
{
    TUint32 offset = melM_op_geta25(i->inst);
    StackEntry* val = melM_stackOffset(&vm->stack, offset);
    assert(val != NULL);

    melM_stackPush(&vm->stack, val);
}

static void opPushIterator(VM* vm, Instruction* i)
{
    TUint32 slot = melM_op_geta25(i->inst);
    Value val = *melM_stackOffset(&vm->stack, 0);

    switch(val.type)
    {
        case MELON_TYPE_OBJECT:
            {
                GCItem* iterator = melResolveSymbolObject(vm, val.pack.obj, MELON_OBJSYM_ITERATOR);

                if (iterator != NULL)
                {
                    melM_stackPop(&vm->stack);

                    melM_vstackPushGCItem(&vm->stack, iterator);
                    
                    if (iterator->type == MELON_TYPE_CLOSURE)
                    {
                        melM_stackPush(&vm->stack, &val);
                        melCallClosureSyncVM(vm, 1, 0, 1);   
                    }
                }
                else
                {
                    melPushIndexedIterator(vm, MELON_TYPE_OBJECT, melObjectNextNativeFunc);   
                }
            }
            break;

        case MELON_TYPE_ARRAY:
            melPushIndexedIterator(vm, MELON_TYPE_ARRAY, melArrayNextNativeFunc);
            break;

        case MELON_TYPE_STRING:
            melPushIndexedIterator(vm, MELON_TYPE_STRING, melStringNextNativeFunc);
            break;

        case MELON_TYPE_RANGE:
            melPushIndexedIterator(vm, MELON_TYPE_RANGE, melRangeNextNativeFunc);
            break;

        case MELON_TYPE_SMALLRANGE:
            melPushIndexedIterator(vm, MELON_TYPE_SMALLRANGE, melRangeNextNativeFunc);
            break;

        default:
            melM_vstackPushNull(&vm->stack);
            break;
    }

    if (slot > 0)
    {
        Value* val = melM_stackPop(&vm->stack);
        CallFrame* cf = melGetTopCallFrameVM(vm);
        melSetLocalVM(vm, cf, slot - 1, val, 0);
    }
}

static void nativeIteratorNext(VM* vm)
{
    NativeIterator* ni = melM_nativeitFromObj(melM_stackPop(&vm->stack)->pack.obj);
    ni->nextFunc(vm, ni);
}

static void iteratorNext(VM* vm)
{
    melSaveAndPauseGC(vm, &vm->gc);

    Value iterator = *melM_stackPop(&vm->stack);
    GCItem* next = melResolveSymbolObject(vm, iterator.pack.obj, MELON_OBJSYM_NEXT);

    if (next == NULL || next->type != MELON_TYPE_CLOSURE)
    {
        melM_fatal(vm, "'next' function not found in iterator or is not a function.");
        return;
    }

    melM_vstackPushGCItem(&vm->stack, next);
    melM_stackPush(&vm->stack, &iterator);
    melRestorePauseGC(vm, &vm->gc);

    melCallClosureSyncVM(vm, 1, 0, 1);
}

static void opIteratorNext(VM* vm)
{
    TType type = melPeekStackType(vm, 0);

    switch(type)
    {
        case MELON_TYPE_OBJECT:
            iteratorNext(vm);
            break;

        case MELON_TYPE_NATIVEIT:
            nativeIteratorNext(vm);
            break;

        default:
            melM_fatal(vm, "Can't get next value from instance of type '%s'", MELON_TYPES_NAMES[type]);
            return;
    }
}

static void opSetupFor(VM* vm, Instruction* i, CallFrame* cf)
{
    TVMInstK valIdx = melM_op_geta13(i->inst);
    TVMInstK itIdx = melM_op_getb12(i->inst);

    Value* val = melGetLocalVM(vm, cf, itIdx, 0);
    assert(val != NULL);
    GCItem* valItem = val->pack.obj;
    melM_stackPush(&vm->stack, val);
    opIteratorNext(vm);

    TBool isNativeIterator = val->type == MELON_TYPE_NATIVEIT;
    GCItem* itResult = NULL;
    Value* done = NULL;

    if (isNativeIterator)
    {
        NativeIterator* ni = melM_nativeitFromObj(valItem);
        itResult = ni->result;
        done = ni->done;
        val = ni->next;
    }
    else
    {
        itResult = melPeekStackOfType(vm, MELON_TYPE_OBJECT, 0)->pack.obj;
        done = melGetValueObject(vm, itResult, &iteratorDoneKey);
    
        if (done != NULL && done->type != MELON_TYPE_BOOL)
        {
            melM_fatal(
                vm, 
                "Wrong 'done' key type in iterator, should be boolean, was '%s'",
                MELON_TYPES_NAMES[done->type]
            );

            return;
        }
    }

    TBool notDone = (done == NULL || !done->pack.value.boolean);

    cf->pc += notDone;

    if (notDone)
    {
        if (!isNativeIterator)
        {
            val = melGetValueObject(vm, itResult, &iteratorValueKey);

            if (val == NULL)
            {
                melM_fatal(
                    vm, 
                    "Missing 'value' key in iterator result when 'done' is false or missing."
                );

                return;
            }
        }

        melSetLocalVM(vm, cf, valIdx, val, 0);
    }
    else if (isNativeIterator)
    {
        NativeIterator* ni = melM_nativeitFromObj(valItem);

        if (
            ni->value.type == MELON_TYPE_RANGE
            || ni->value.type == MELON_TYPE_SMALLRANGE
        )
        {
            melPushRangeIteratorPool(&vm->gc, valItem);
        }
    }

    // Pops the NEXTITER result object
    melM_stackPop(&vm->stack);
}

/**
 * This function invokes a given custom operator on the object
 * with some arguments.
 * 
 * A the moment invoking a custom operator incurs in a performance
 * penalty that is a consequence of how things are laid out on 
 * the stack when this function is called.
 * We usually have the arguments before this function but if
 * a custom operator has been found we have to put the operator
 * Closure *before* the arguments. There's also another issue:
 * If the custom operator returns false it means that code wants
 * the VM to fall back to the standard operator. In this case
 * we have to restore things on the stack as they were before
 * this function was called.
 * 
 * To work around both problems we:
 * - Push the closure on the stack
 * - Copy the arguments after the closure
 * - If the closure returned true it means we can leave things as they are
 * - If the closure returned false it means we have to restore things as
 *   they were. We simply pop the 'false' value and pop the extra arguments.
 * - If the closure returned a non boolean value we have to:
 *   = Pop and save the result (disabling GC)
 *   = Remove the extra arguments
 *   = Push again the result so that it's on the top of the stack as expected
 * 
 * As you see there's quite some extra stuff going on.
 **/

TRet melPerformCustomOpVM(VM* vm, MelonObjectSymbols op, TUint8 args)
{
	assert(args > 0);

    Value* a = melPeekStackOfType(vm, MELON_TYPE_OBJECT, args - 1);

    GCItem* operator = melResolveSymbolObject(vm, a->pack.obj, op);

    if (operator == NULL)
    {
        return 1;
    }

	Closure* operatorCl = melM_closureFromObj(operator);
	assert(operatorCl != NULL);
    Function* operatorFn = melM_functionFromObj(operatorCl->fn);
	
	TSize oldTop = vm->stack.top;
	
	melM_vstackPushGCItem(&vm->stack, operator);

	for (TUint8 i = (operatorFn->method ? args : args - 1); i > 0; --i)
	{
		melM_stackPush(&vm->stack, melM_stackGet(&vm->stack, oldTop - (i - 1) - 1));
	}

    melCallClosureSyncVM(vm, args - (operatorFn->method ? 0 : 1), 0, 1);

    TType resType = melPeekStackType(vm, 0);

	if (resType == MELON_TYPE_BOOL)
	{
		Value* res = melM_stackPop(&vm->stack);

		if (!res->pack.value.boolean)
		{
			return 1;
		}

		melM_stackPopCount(&vm->stack, args);
	}
    else if (resType == MELON_TYPE_NULL)
    {
        // If the function returned nothing or null
        // we simply discard it.

        melM_stackPop(&vm->stack);
    }
	else
	{
		TSize resultsCount = vm->stack.top - oldTop;
		assert(resultsCount <= 1);

		if (resultsCount > 0)
		{
			melSaveAndPauseGC(vm, &vm->gc);	
			Value result = *melM_stackPop(&vm->stack);
			melM_stackPopCount(&vm->stack, args);
			melM_stackPushRaw(&vm->stack, &result);
			melRestorePauseGC(vm, &vm->gc);
		}
	}

    return 0;
}

static void opDumpInfo(VM* vm, Instruction* i)
{
    TUint32 num = melM_op_geta25(i->inst);

    switch(num)
    {
        case 0:
            melPrintStackUtils(vm);
            return;

        case 1:
            melPrintCallStackUtils(vm);
            return;

        case 2:
            melPrintGCInfoUtils(vm, 0);
            return;

        case 3:
            melPrintUpvaluesUtils(vm);
            return;
    }
}

static TSize melGetSlotToStack(VM* vm, CallFrame* cf, TSize slot, TBool ignoreMethod)
{
    assert(cf != NULL);
    assert(slot < cf->function->args + cf->function->localsSlots);

    return cf->stackStart + slot + (ignoreMethod ?  0 : cf->function->method);
}

TRet melSetLocalVM(VM* vm, CallFrame* cf, TSize slot, Value* val, TBool ignoreMethod)
{
    melM_stackSet(&vm->stack, melGetSlotToStack(vm, cf, slot, ignoreMethod), val);
    return 0;
}

Value* melGetLocalVM(VM* vm, CallFrame* cf, TSize slot, TBool ignoreMethod)
{
    StackEntry* se = melM_stackGet(&vm->stack, melGetSlotToStack(vm, cf, slot, ignoreMethod));
    return se;
}

static Value* melPopStackType(VM* vm, TType type)
{
    StackEntry* se = melM_stackTop(&vm->stack);
    assert(se != NULL);

    if (se->type != type)
    {
        melM_fatal(vm, "Expected type '%s' on the stack but found '%s'", MELON_TYPES_NAMES[type], MELON_TYPES_NAMES[se->type]);
        return NULL;
    }

    melM_stackPop(&vm->stack);

    return se;
}

static TType melPeekStackType(VM* vm, StackSize offset)
{
    return melM_stackGet(&vm->stack, vm->stack.top - offset - 1)->type;
}

static Value* melPeekStackOfType(VM* vm, TType type, StackSize offset)
{
    StackEntry* se = melM_stackOffset(&vm->stack, offset);

    if (se == NULL)
    {
        return NULL;
    }

    if (se->type != type)
    {
        melM_fatal(vm, "Expected type '%s' on the stack at %d but found '%s'", MELON_TYPES_NAMES[type], offset, MELON_TYPES_NAMES[se->type]);
        return NULL;
    }

    return se;
}

static Value* getConstant(VM* vm, TConstantID k)
{
    Function* fn = melGetTopCallFrameVM(vm)->function;

    if (fn->constants.count <= k)
    {
        return NULL;
    }

    return &fn->constants.constants[k];
}

static TRet melNewCallFrame(VM* vm, TUint16 expRet)
{
    melM_stackEnsure(&vm->callStack, vm->callStack.top + 1);
    melM_stackSetTop(&vm->callStack, vm->callStack.top + 1);

    memset(melM_stackTop(&vm->callStack), 0, sizeof(CallFrame));
    melM_stackTop(&vm->callStack)->expRet = expRet;

    return 0;
}

static TRet melGenerateHashKey(VM* vm)
{
    srand(time(NULL));

    for (TByte i = 0; i < MELON_VM_SIPHASH_KEY_SIZE; i++)
    {
        vm->hashKey[i] = random() % 256;
    }

    return 0;
}

static TRet onStackInvalidated(void* context, StackEntry* oldStack)
{
    assert(context != NULL);
    VM* vm = (VM*)context;

    Upvalue* uv = vm->openUpvalues;

    while (uv != NULL)
    {
        uv->value = vm->stack.stack + (uv->value - oldStack);
        uv = uv->next;
    }

    return 0;
}

static void melPushNativeIteratorResult(VM* vm, NativeIterator* nit, Value* val, TBool done, StackSize popCount)
{
    if (nit->result == NULL)
    {
        nit->result = melNewObject(vm);

        melWriteBarrierGC(vm, melM_nativeitToObj(nit), nit->result);

        if (val == NULL)
        {
            val = &nullValue;
        }

        melSetValueObject(vm, nit->result, &iteratorValueKey, val);
        nit->next = melGetValueObject(vm, nit->result, &iteratorValueKey);
        
        Value doneVal;
        doneVal.type = MELON_TYPE_BOOL;
        doneVal.pack.value.boolean = done;
        melSetValueObject(vm, nit->result, &iteratorDoneKey, &doneVal);
        nit->done = melGetValueObject(vm, nit->result, &iteratorDoneKey);
    }
    else
    {
        *nit->next = *val;
        nit->done->pack.value.boolean = done;
    }
    
    melM_stackPopCount(&vm->stack, popCount);
    melM_vstackPushGCItem(&vm->stack, nit->result);
}

static TByte melPushIndexedIterator(
    VM* vm, 
    TType type,
    NativeIteratorNext nextFunc
)
{
    Value* inst = melPeekStackOfType(vm, type, 0);
    GCItem* nativeIt = NULL;

    if (
        inst->type == MELON_TYPE_RANGE
        || inst->type == MELON_TYPE_SMALLRANGE
    )
    {
        melPopRangeIteratorPool(&vm->gc, inst, &nativeIt);
    }
    
    if (nativeIt == NULL)
    {
        nativeIt = melNewNativeIterator(vm, inst, nextFunc, sizeof(NativeIndexedIterator));
    }

    assert(nativeIt != NULL);

    NativeIndexedIterator* nativeItObj = (NativeIndexedIterator*)melM_nativeitFromObj(nativeIt);
    nativeItObj->index = 0;

    // Let's recycle the stack slot
    inst->type = nativeIt->type;
    inst->pack.obj = nativeIt;

    return 1;
}

static void melIteratorNextNativeFunc(
    VM* vm, 
    NativeIterator* ni,
    Value (*addItemFunc)(VM*, const Value*, TInteger, StackSize*)
)
{
    assert(ni->size == sizeof(NativeIndexedIterator));
    NativeIndexedIterator* nii = (NativeIndexedIterator*)ni;

    StackSize popCount = 0;
    Value result = addItemFunc(vm, &nii->value, nii->index, &popCount);
    nii->index++;

    TBool done = result.type == MELON_TYPE_NONE;

    if (done)
    {
        // Last value can still be read and should be null
        result.type = MELON_TYPE_NULL;
    }

    melPushNativeIteratorResult(vm, (NativeIterator*)nii, &result, done, popCount);
}

static Value returnNextArrayItem(VM* vm, const Value* obj, TInteger idx, StackSize* popCount)
{
    Value* val = melGetIndexArray(vm, obj->pack.obj, idx);

    if (val == NULL)
    {
        return noneValue;
    }

    return *val;
}

static TRet melArrayNextNativeFunc(VM* vm, NativeIterator* ni)
{
    melIteratorNextNativeFunc(vm, ni, &returnNextArrayItem);
    return 0;
}

static Value returnNextStringItem(VM* vm, const Value* obj, TInteger idx, StackSize* popCount)
{
    String* strObj = melM_strFromObj(obj->pack.obj);

    if (idx >= strObj->len)
    {
        return noneValue;
    }

    const char* strData = melM_strDataFromObj(obj->pack.obj);

    *popCount = 1;

    Value val;
    val.type = MELON_TYPE_STRING;
    val.pack.obj = melNewString(vm, &strData[idx], 1);

    // Prevents the String from GCing
    melM_stackPush(&vm->stack, &val);

    return val;
}

static TRet melStringNextNativeFunc(VM* vm, NativeIterator* ni)
{
    melIteratorNextNativeFunc(vm, ni, &returnNextStringItem);
    return 0;
}

static Value returnNextObjectItem(VM* vm, const Value* obj, TInteger idx, StackSize* popCount)
{
    Value* val = melGetKeyAtIndexObject(vm, obj->pack.obj, idx, 0);

    if (val == NULL)
    {
        return noneValue;
    }

    return *val;
}

static TRet melObjectNextNativeFunc(VM* vm, NativeIterator* ni)
{
    melIteratorNextNativeFunc(vm, ni, &returnNextObjectItem);
    return 0;
}

static Value returnNextRangeItem(VM* vm, const Value* obj, TInteger idx, StackSize* popCount)
{
    TInteger start = 0;
    TInteger end = 0;

    if (obj->type == MELON_TYPE_SMALLRANGE)
    {
        start = melM_rangeSmallStart(obj);
        end = melM_rangeSmallEnd(obj);
    }
    else
    {
        Range* r = melM_rangeFromObj(obj);
        start = r->start;
        end = r->end;
    }

    if (start == end)
    {
        return noneValue;
    }

    TInteger newVal;

    if (start < end)
    {
        newVal = start + idx;

        if (newVal >= end)
        {
            return noneValue;
        }
    }
    else
    {
        newVal = start - idx - 1;
        
        if (newVal < end)
        {
            return noneValue;
        }
    }

    Value val;
    val.type = MELON_TYPE_INTEGER;
    val.pack.value.integer = newVal;

    return val;
}

static TRet melRangeNextNativeFunc(VM* vm, NativeIterator* ni)
{
    melIteratorNextNativeFunc(vm, ni, &returnNextRangeItem);
    return 0;
}

static TRet melCreateNativeIterators(VM* vm)
{
    // @todo: Can by a bit DRYer

    nativeIteratorValueSymbol.type = MELON_TYPE_SYMBOL;
    nativeIteratorValueSymbol.pack.obj = melNewSymbol(vm, NULL);
    melAddRootGC(vm, &vm->gc, nativeIteratorValueSymbol.pack.obj);

    nativeIteratorIdxSymbol.type = MELON_TYPE_SYMBOL;
    nativeIteratorIdxSymbol.pack.obj = melNewSymbol(vm, NULL);
    melAddRootGC(vm, &vm->gc, nativeIteratorIdxSymbol.pack.obj);

    iteratorValueKey.type = MELON_TYPE_STRING;
    iteratorValueKey.pack.obj = melNewString(
        vm, 
        MELON_VM_ITERATOR_VALUE_PROPERTY, 
        strlen(MELON_VM_ITERATOR_VALUE_PROPERTY)
    );
    melAddRootGC(vm, &vm->gc, iteratorValueKey.pack.obj);

    iteratorDoneKey.type = MELON_TYPE_STRING;
    iteratorDoneKey.pack.obj = melNewString(
        vm, 
        MELON_VM_ITERATOR_DONE_PROPERTY, 
        strlen(MELON_VM_ITERATOR_DONE_PROPERTY)
    );
    melAddRootGC(vm, &vm->gc, iteratorDoneKey.pack.obj);

    return 0;
}

static TRet melCreateGlobal(VM* vm)
{
    vm->global = melNewObject(vm);
    melAddRootGC(vm, &vm->gc, vm->global);
    return 0;
}

TRet melCreateVM(VM* vm, VMConfig* config)
{
    if (melCreateStringsTable(&vm->stringsTable) != 0)
    {
        melM_fatal(vm, "Can't create strings table");
        return 1;
    }

    if (melCreateStack(&vm->stack, MELON_VM_MIN_STACK_SIZE, sizeof(StackEntry)) != 0)
    {
        melM_fatal(vm, "Can't allocate VM's stack");
        return 1;
    }

    vm->config = *config;

    // Defaults to the default print function
    // if none was specified
    if (vm->config.printFn == NULL)
    {
        vm->config.printFn = &melVMPrintFunctionUtils;
    }

    if (melCreateStack(&vm->callStack, MELON_VM_MIN_CALLSTACK_SIZE, sizeof(CallFrame)) != 0)
    {
        melM_fatal(vm, "Can't allocate call stack");
        return 1;
    }

    vm->openUpvalues = NULL;
    vm->nextSymbolId = MELON_VM_FIRST_SYMBOL_ID;

    nullValue.type = MELON_TYPE_NULL;

    if (melInitGC(&vm->gc) != 0)
    {
        melM_fatal(vm, "Can't init GC");
        return 1;
    }

    if (melGenerateHashKey(vm) != 0)
    {
        melM_fatal(vm, "Can't generate hash key");
        return 1;
    }

    if (melInitSystemObject(vm) != 0)
    {
        melM_fatal(vm, "Can't initialize object system");
        return 1;
    }

    if (melCreateGlobal(vm) != 0)
    {
        melM_fatal(vm, "Can't init global object");
        return 1;
    }

    if (melCreateNativeIterators(vm) != 0)
    {
        melM_fatal(vm, "Can't init native iterators");
        return 1;
    }

    if (melRegisterModules(vm, MEL_SYSTEM_MODULES) != 0)
    {
        melM_fatal(vm, "Can't initialize system modules");
        return 1;
    }

    return 0;
}

static void _deserializerError(void* ctx, const char* str)
{
    melErrorVM((VM*)ctx, "%s", str);
}

TRet melLoadProgramVM(
    VM* vm, 
    TByte* program,
    TSize size,
    GCItem** prog,
    const char* path,
    Deserializer* ds
)
{
    if (size < sizeof(struct ProgramHeader))
    {
        melM_fatal(vm, "Invalid program (Invalid program size).");
        return 1;
    }

    Deserializer d;
    d.error = &_deserializerError;

    if (ds == NULL)
    {
        ds = &d;
    }

    ds->buffer = program;
    ds->count = size;
    ds->ctx = vm;
    ds->cur = 0;
    ds->vm = vm;

    melSetPauseGC(vm, &vm->gc, 1);
    
    GCItem* pi = melNewProgram(vm);
    Program* p = melM_programFromObj(pi);

    melAddRootGC(vm, &vm->gc, pi);

    if (melDeserializerReadHeader(ds, &p->header) != 0)
    {
        return 1;
    }

    if (melDeserializerReadGCItem(ds, &p->main) != 0)
    {
        return 1;
    }

    *prog = pi;

    Function* mainFn = melM_functionFromObj(p->main);

    if (path)
    {
        mainFn->debug.file = malloc(sizeof(char) * strlen(path) + 1);
        strcpy((char*)mainFn->debug.file, path);
    }
    else
    {
        mainFn->debug.file = NULL;
    }

    melSetPauseGC(vm, &vm->gc, 0);
    
    return 0;
}

static Upvalue* melAddUpvalue(VM* vm, Value* value)
{
    Upvalue* uv = malloc(sizeof(Upvalue));
    assert(uv != NULL);

    uv->value = value;
    uv->refCount = 0;

    vm->gc.usedBytes += sizeof(Upvalue);
    vm->gc.whitesCount++;

    return uv;
}

static TRet melFreeUpvalue(VM* vm, Upvalue* uv)
{
#ifdef _ZERO_MEMORY_ON_FREE_GC
    memset(uv, 0, sizeof(Upvalue));
#endif

    free(uv);

    vm->gc.usedBytes -= sizeof(Upvalue);

    return 0;
}

Upvalue* melGetOrAddUpvalueVM(VM* vm, Value* value)
{
    Upvalue* curUv = vm->openUpvalues;
    Upvalue* lastUv = NULL;

    while(curUv != NULL && curUv->value > value)
    {
        lastUv = curUv;
        curUv = curUv->next;
    }

    if (curUv != NULL && curUv->value == value)
    {
        return curUv;
    }

    Upvalue* uv = melAddUpvalue(vm, value);

    // It's the first or smallest upvalue and it belongs to the tail
    if (curUv == NULL)
    {
        uv->prev = lastUv;
        uv->next = NULL;

        if (lastUv != NULL)
        {
            lastUv->next = uv;
        }
        else
        {
            vm->openUpvalues = uv;
        }

        return uv;
    }

    // Must be inserted before curUv but after
    // curUv->prev (if any)
    if (curUv->prev == NULL)
    {
        vm->openUpvalues = uv;
    }
    else
    {
        curUv->prev->next = uv;
    }

    uv->prev = curUv->prev;
    uv->next = curUv;
    curUv->prev = uv;

    return uv;
}

TRet melCloseUpvaluesVM(VM* vm, Value* upToValue)
{
    Upvalue* curUv = NULL;

    while (vm->openUpvalues != NULL && vm->openUpvalues->value >= upToValue)
    {
        curUv = vm->openUpvalues;
        
        curUv->closed = *curUv->value;
        curUv->value = &curUv->closed;

        vm->openUpvalues = curUv->next;

        if (vm->openUpvalues)
        {
            vm->openUpvalues->prev = NULL;
            
            if (vm->openUpvalues->next)
            {
                vm->openUpvalues->next->prev = vm->openUpvalues;
            }
        }

        curUv->prev = NULL;
        curUv->next = NULL;
    }

    return 0;
}

TRet melReleaseUpvalueVM(VM* vm, Upvalue* upvalue)
{
    assert(upvalue->refCount > 0);
    upvalue->refCount--;

    if (upvalue->refCount == 0)
    {
        Upvalue** prevNext = upvalue->prev != NULL ? &upvalue->prev->next : &vm->openUpvalues;

        *prevNext = upvalue->next;

        if (upvalue->next != NULL)
        {
            upvalue->next->prev = upvalue->prev;
        }
        
        melFreeUpvalue(vm, upvalue);
    }

    return 0;
}

TRet melCallFunctionVM(VM* vm, GCItem* cl, GCItem* fObj, TUint16 expRet)
{
    if (melNewCallFrame(vm, expRet) != 0)
    {
        melM_fatal(vm, "Can't create new callstack frame. (stack overflow?)");
        return 1;
    }

    assert(fObj->type == MELON_TYPE_FUNCTION);

    Function* f = melM_functionFromObj(fObj);
    assert(vm->stack.top >= f->args);

    CallFrame* cf = melGetTopCallFrameVM(vm);
    cf->function = f;
    cf->stackStart = vm->stack.top - f->args;
    cf->closure = cl;
    cf->pc = 0;

    melM_stackEnsure(&vm->stack, vm->stack.top + f->localsSlots);
    melM_stackSetTop(&vm->stack, vm->stack.top + f->localsSlots);

    cf->stackBase = vm->stack.top;

    // Initialize the slots to null in case they are GCed
    // before they are assigned a value
    for (TSize i = 0; i < f->localsSlots; i++)
    {
        melM_stackGet(&vm->stack, cf->stackBase - i - 1)->type = MELON_TYPE_NULL;
    }

    if (f->native != NULL)
    {
        melReturnFunction(vm, f->native(vm));
        return 0;
    }

    return 0;
}

TRet melPushModuleObjectVM(VM* vm, GCItem* fnObj)
{
    GCItem* moduleObj = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, moduleObj);

    Function* f = melM_functionFromObj(fnObj);

    Value pathValue;

    if (f->debug.file != NULL)
    {
        pathValue.type = MELON_TYPE_STRING;
        pathValue.pack.obj = melNewString(vm, f->debug.file, strlen(f->debug.file));
    }
    else
    {
        pathValue.type = MELON_TYPE_NULL;
    }

    melM_stackPush(&vm->stack, &pathValue);
    
    Value key;
    key.type = MELON_TYPE_STRING;
    key.pack.obj = melNewString(vm, "path", strlen("path"));

    melSetValueObject(vm, moduleObj, &key, &pathValue);
    melM_stackPop(&vm->stack);

    return 0;
}

static StackEntry* getClosureFromCall(VM* vm, TSize* callArgs)
{
    StackEntry* closurePtr = melM_stackGet(&vm->stack, vm->stack.top - *callArgs - 1);

    if (
        closurePtr == NULL || 
        (closurePtr->type != MELON_TYPE_CLOSURE && closurePtr->type != MELON_TYPE_OBJECT)
    )
    {
        melM_fatal(vm, "Expected closure for function call");
        return NULL;
    }

    if (closurePtr->type == MELON_TYPE_OBJECT)
    {
        GCItem* operator = melResolveSymbolObject(vm, closurePtr->pack.obj, MELON_OBJSYM_CALL);

        if (operator == NULL)
        {
            melM_fatal(vm, "Calling object with no call operator specified.");
            return NULL;
        }

        Closure* cl = melM_closureFromObj(operator);
        Function* fn = melM_functionFromObj(cl->fn);

        if (fn->method == 1)
        {
            melM_stackInsert(&vm->stack, *callArgs, closurePtr);
            ++*callArgs;
        }

        closurePtr->type = MELON_TYPE_CLOSURE;
        closurePtr->pack.obj = operator;
    }

    return closurePtr;
}

static void pushRestArray(VM* vm, TSize count)
{
    melSaveAndPauseGC(vm, &vm->gc);

    GCItem* restArray = melNewArray(vm);

    for (TInt32 i = 0; i < count; i++)
    {
        melPushArray(vm, restArray, melM_stackOffset(&vm->stack, count - i - 1));
    }

    melM_stackPopCount(&vm->stack, count);
    melM_vstackPushGCItem(&vm->stack, restArray);

    melRestorePauseGC(vm, &vm->gc);
}

TRet melCallClosureVM(VM* vm, TSize callArgs, TBool tail, TUint16 expRet)
{
    CallFrame* cf = melGetTopCallFrameVM(vm);

    StackEntry* closurePtr = getClosureFromCall(vm, &callArgs);

    // NOTE: We are copying this here because we are modifying
    //       the stack below and we may invalidate the pointer
    Value closure = *closurePtr;

    Closure* cl = melM_closureFromObj(closure.pack.obj);
    assert(cl->fn != NULL);

    Function* fn = melM_functionFromObj(cl->fn);

    if (fn->method)
    {
        StackEntry* thisInst = melM_stackGet(&vm->stack, vm->stack.top - callArgs);

        if (thisInst == NULL || thisInst->type != MELON_TYPE_OBJECT)
        {
            melM_fatal(vm, "Methods must be called with the method access operator '->'.");
            return 1;
        }
    }

    if (tail)
    {
        melReturnFunction(vm, callArgs + 1);
    }

    TInt32 argsDiff = fn->args - callArgs;

    for (TInt32 i = 0; i < argsDiff; i++)
    {
        melM_vstackPushNull(&vm->stack);
        callArgs++;
    }

    if (fn->rest == 0)
    {
        for (TInt32 i = argsDiff; i < 0; i++)
        {
            melM_stackPop(&vm->stack);
        }
    }
    else
    {
        pushRestArray(vm, callArgs - (fn->args - 1));
    }

    melCallFunctionVM(vm, closure.pack.obj, cl->fn, expRet);

    return 0;
}

TRet melCallClosureSyncVM(VM* vm, TSize callArgs, TBool tail, TUint16 expRet)
{
    TSize callArgsTmp = callArgs;
    StackEntry* closurePtr = getClosureFromCall(vm, &callArgsTmp);
    Closure* cl = melM_closureFromObj(closurePtr->pack.obj);
    Function* fn = melM_functionFromObj(cl->fn);

    TBool native = fn->native != NULL;

    if (melCallClosureVM(vm, callArgs, tail, expRet) != 0)
    {
        return 1;
    }

    if (!native)
    {
        melStepOutOfFunction(vm);
    }

    return 0;
}

static TRet melReturnFunction(VM* vm, TUint32 valuesCount)
{
    assert(!melM_stackIsEmpty(&vm->callStack));

    CallFrame* cf = melGetTopCallFrameVM(vm);
    assert(cf != NULL);

    if (vm->openUpvalues != NULL)
    {
        melCloseUpvaluesVM(vm, melM_stackGet(&vm->stack, cf->stackStart - 1));
    }

    assert(cf->stackStart > 0);

    if (cf->expRet == 0 && valuesCount > 0)
    {
        // Discard the results
        valuesCount = 0;
    }

    if (valuesCount > 0)
    {
        melM_stackEnsure(&vm->stack, cf->stackStart + valuesCount - 1);

        if (valuesCount == 1)
        {
            vm->stack.stack[cf->stackStart - 1] = vm->stack.stack[vm->stack.top - 1];
        }
        else
        {
            StackEntry* retStart = &vm->stack.stack[vm->stack.top - valuesCount];
            StackEntry* newRetStart = &vm->stack.stack[cf->stackStart - 1];
            memcpy(newRetStart, retStart, sizeof(StackEntry) * valuesCount);
        }
    }
    else if (cf->expRet == 1)
    {
        valuesCount = 1;
        vm->stack.stack[cf->stackStart - 1].type = MELON_TYPE_NULL;
    }

    melM_stackSetTop(&vm->stack, cf->stackStart + valuesCount - 1);
    melM_stackPop(&vm->callStack);

    return 0;
}

CallFrame* melGetTopCallFrameVM(VM* vm)
{
    return melM_stackOffset(&vm->callStack, 0);
}

TRet melStepOutOfFunction(VM* vm)
{
	TSize depth = vm->callStack.top;

    Value tmpV;
    TByte opCode;

    CallFrame* cf = &vm->callStack.stack[vm->callStack.top - 1];
    Instruction* code = cf->function->code;
    Instruction* inst = NULL;

    const void* jmps[] = {
        &&MELON_OP_NOP,
        &&MELON_OP_ADD,
        &&MELON_OP_SUB,
        &&MELON_OP_MUL,
        &&MELON_OP_DIV,
        &&MELON_OP_PUSHK,
        &&MELON_OP_CONCAT,
        &&MELON_OP_HALT,
        &&MELON_OP_PUSHNULL,
        &&MELON_OP_PUSHTRUE,
        &&MELON_OP_PUSHFALSE,
        &&MELON_OP_JMP,
        &&MELON_OP_TESTTRUE,
        &&MELON_OP_TESTFALSE,
        &&MELON_OP_EQ,
        &&MELON_OP_LTE,
        &&MELON_OP_GTE,
        &&MELON_OP_GT,
        &&MELON_OP_LT,
        &&MELON_OP_NEWOBJ,
        &&MELON_OP_SETOBJ,
        &&MELON_OP_GETOBJ,
        &&MELON_OP_PUSHLOC,
        &&MELON_OP_STORELOC,
        &&MELON_OP_CALL,
        &&MELON_OP_RET,
        &&MELON_OP_CALLTAIL,
        &&MELON_OP_PUSHI,
        &&MELON_OP_NEWCL,
        &&MELON_OP_DUMPINFO,
        &&MELON_OP_PUSHUPVAL,
        &&MELON_OP_STOREUPVAL,
        &&MELON_OP_POP,
        &&MELON_OP_GC,
        &&MELON_OP_TESTNULL,
        &&MELON_OP_PUSH,
        &&MELON_OP_NEWARR,
        &&MELON_OP_GETARR,
        &&MELON_OP_GETIARR,
        &&MELON_OP_SETARR,
        &&MELON_OP_SETIARR,
        &&MELON_OP_PUSHARR,
        &&MELON_OP_SIZEARR,
        &&MELON_OP_OR,
        &&MELON_OP_AND,
        &&MELON_OP_NOT,
        &&MELON_OP_SETGLOBAL,
        &&MELON_OP_GETGLOBAL,
        &&MELON_OP_SLICE,
        &&MELON_OP_BLESS,
        &&MELON_OP_PUSHTHIS,
        &&MELON_OP_NEG,
        &&MELON_OP_NEWSYMBOL,
        &&MELON_OP_PUSHBLESSING,
        &&MELON_OP_PUSHITER,
        &&MELON_OP_NEXTITER,
        &&MELON_OP_SETUPFOR,
        &&MELON_OP_BITLSHIFT,
        &&MELON_OP_BITRSHIFT,
        &&MELON_OP_BITAND,
        &&MELON_OP_BITOR,
        &&MELON_OP_BITNOT,
        &&MELON_OP_BITXOR,
        &&MELON_OP_POW,
        &&MELON_OP_MOD,
        &&MELON_OP_STORETOPCL,
        &&MELON_OP_COALESCE,
    };

    FETCH_DECODE:
        assert(cf->pc < (cf->function->size / sizeof(TVMInst)));
        inst = &code[cf->pc];
        opCode = melM_getOpCode(inst->inst);
        cf->pc++;

        goto *jmps[opCode];

        // switch (opCode)
        // {
        MELON_OP_NOP:
            goto FETCH_DECODE;

        MELON_OP_ADD:
            opADD(vm);
            goto FETCH_DECODE;

        MELON_OP_SUB:
            opSUB(vm);
            goto FETCH_DECODE;

        MELON_OP_MUL:
            opMUL(vm);
            goto FETCH_DECODE;

        MELON_OP_DIV:
            opDIV(vm);
            goto FETCH_DECODE;

        MELON_OP_PUSHK:
            opPushK(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_PUSHI:
            opPushI(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_CONCAT:
            opConcat(vm);
            goto FETCH_DECODE;

        MELON_OP_PUSHNULL:
            melM_vstackPushNull(&vm->stack);
            goto FETCH_DECODE;

        MELON_OP_PUSHTRUE:
            tmpV.type = MELON_TYPE_BOOL;
            tmpV.pack.value.boolean = 1;
            melM_stackPush(&vm->stack, &tmpV);
            goto FETCH_DECODE;

        MELON_OP_PUSHFALSE:
            tmpV.type = MELON_TYPE_BOOL;
            tmpV.pack.value.boolean = 0;
            melM_stackPush(&vm->stack, &tmpV);
            goto FETCH_DECODE;

        MELON_OP_JMP:
            opJmp(vm, inst, &cf->pc);
            goto FETCH_DECODE;

        MELON_OP_TESTTRUE:
            opTestTrue(vm, inst, &cf->pc);
            goto FETCH_DECODE;

        MELON_OP_TESTFALSE:
            opTestFalse(vm, inst, &cf->pc);
            goto FETCH_DECODE;

        MELON_OP_TESTNULL:
            opTestNull(vm);
            goto FETCH_DECODE;

        MELON_OP_EQ:
            opEq(vm);
            goto FETCH_DECODE;

        MELON_OP_LTE:
            opCmp(vm, MELON_OP_LTE);
            goto FETCH_DECODE;

        MELON_OP_GTE:
            opCmp(vm, MELON_OP_GTE);
            goto FETCH_DECODE;

        MELON_OP_GT:
            opCmp(vm, MELON_OP_GT);
            goto FETCH_DECODE;

        MELON_OP_LT:
            opCmp(vm, MELON_OP_LT);
            goto FETCH_DECODE;
            
        MELON_OP_HALT:
            return 1;

        MELON_OP_NEWOBJ:
            opNewObj(vm);
            goto FETCH_DECODE;

        MELON_OP_SETOBJ:
            opSetObj(vm);
            goto FETCH_DECODE;

        MELON_OP_GETOBJ:
            opGetObj(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_SETGLOBAL:
            opSetGlobal(vm);
            goto FETCH_DECODE;

        MELON_OP_GETGLOBAL:
            opGetGlobal(vm);
            goto FETCH_DECODE;

        MELON_OP_PUSHLOC:
            opLoad(vm, inst, cf);
            goto FETCH_DECODE;

        MELON_OP_STORELOC:
            opStore(vm, inst, cf);
            goto FETCH_DECODE;

        MELON_OP_CALL:
            // @NOTE: we increment pc here
            //        because the cf pointer may be
            //        invalidated by opCall ending up
            //        reallocing the call stack
            opCall(vm, inst, 0);
            cf = melM_stackTop(&vm->callStack);
            code = cf->function->code;
            goto FETCH_DECODE;

        MELON_OP_CALLTAIL:
            opCall(vm, inst, 1);
            goto FETCH_DECODE; // We don't want pc to increment

        MELON_OP_NEWCL:
            opNewClosure(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_RET:
            opReturn(vm, inst);
            
            if (vm->callStack.top < depth)
            {
                return 0;
            }

            cf = melM_stackTop(&vm->callStack);
            code = cf->function->code;
            goto FETCH_DECODE;

        MELON_OP_DUMPINFO:
            opDumpInfo(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_PUSHUPVAL:
            opPushUpval(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_STOREUPVAL:
            opStoreUpval(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_POP:
            melM_stackPop(&vm->stack);
            goto FETCH_DECODE;

        MELON_OP_PUSH:
            opPush(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_NEWARR:
            opNewArray(vm);
            goto FETCH_DECODE;

        MELON_OP_GETARR:
            opGetArray(vm);
            goto FETCH_DECODE;

        MELON_OP_GETIARR:
            opGetIArray(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_SETARR:
            opSetArray(vm);
            goto FETCH_DECODE;

        MELON_OP_SETIARR:
            opSetIArray(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_PUSHARR:
            opPushArray(vm);
            goto FETCH_DECODE;

        MELON_OP_SIZEARR:
            opSizeArray(vm);
            goto FETCH_DECODE;

        MELON_OP_GC:
            melTriggerGC(vm, &vm->gc);
            goto FETCH_DECODE;

        MELON_OP_OR:
            opLogicOr(vm);
            goto FETCH_DECODE;

        MELON_OP_AND:
            opLogicAnd(vm);
            goto FETCH_DECODE;

        MELON_OP_NOT:
            opLogicNot(vm);
            goto FETCH_DECODE;

        MELON_OP_SLICE:
            opSlice(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_BLESS:
            opBless(vm);
            goto FETCH_DECODE;

        MELON_OP_PUSHTHIS:
            opPushThis(vm, cf);
            goto FETCH_DECODE;

        MELON_OP_NEG:
            opNEG(vm);
            goto FETCH_DECODE;

        MELON_OP_NEWSYMBOL:
            opNewSymbol(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_PUSHBLESSING:
            opPushBlessing(vm);
            goto FETCH_DECODE;

        MELON_OP_PUSHITER:
            opPushIterator(vm, inst);
            goto FETCH_DECODE;

        MELON_OP_NEXTITER:
            opIteratorNext(vm);
            goto FETCH_DECODE;

        MELON_OP_SETUPFOR:
            opSetupFor(vm, inst, cf);
            goto FETCH_DECODE;

        MELON_OP_BITLSHIFT:
            opBitLeftShiftInt(vm);
            goto FETCH_DECODE;

        MELON_OP_BITRSHIFT:
            opBitRightShiftInt(vm);
            goto FETCH_DECODE;

        MELON_OP_BITAND:
            opBitAndInt(vm);
            goto FETCH_DECODE;

        MELON_OP_BITOR:
            opBitOrInt(vm);
            goto FETCH_DECODE;

        MELON_OP_BITNOT:
            opBitNotInt(vm);
            goto FETCH_DECODE;

        MELON_OP_BITXOR:
            opBitXorInt(vm);
            goto FETCH_DECODE;

        MELON_OP_MOD:
            opModInt(vm);
            goto FETCH_DECODE;

        MELON_OP_POW:
            opPow(vm);
            goto FETCH_DECODE;

        MELON_OP_STORETOPCL:
            opStoreTopCL(vm, inst, cf);
            goto FETCH_DECODE;

        MELON_OP_COALESCE:
            opCoalesce(vm);
            goto FETCH_DECODE;

	return 0;
}

TRet melRunProgramVM(VM* vm, GCItem* pi)
{
    if (pi->type != MELON_TYPE_PROGRAM)
    {
        return 1;
    }

    Program* p = melM_programFromObj(pi);

    return melRunMainFunctionVM(vm, p->main);
}

static TRet melRunEntryPointFunctionVM(VM* vm, GCItem* f, TUint16 expRet)
{
    GCItem* mainCl = melNewClosure(vm, f);

    melM_vstackPushGCItem(&vm->stack, mainCl);
    melPushModuleObjectVM(vm, f);

    melCallClosureSyncVM(vm, 1, 0, expRet);

    return 0;
}

TRet melRunMainFunctionVM(VM* vm, GCItem* f)
{
    TRet res = melRunEntryPointFunctionVM(vm, f, 0);

#ifdef _PRINT_GSTATS_GC
    melDumpGlobalStatsGC(&vm->gc);
#endif

    return res;
}

TRet melRunModuleFunctionVM(VM* vm, GCItem* f)
{
    return melRunEntryPointFunctionVM(vm, f, 1);
}

void melErrorVM(VM* vm, const char *format, ...)
{
    // This can be NULL if an error has been raised
    // before the VM even run (eg: deserialization)
    CallFrame* cf = melGetTopCallFrameVM(vm);

    // If the function is native we do not have debug informations
    // so we are simply falling back to the function call itself
    if (cf != NULL && cf->function->native)
    {
        // Maybe this native functionw as invoked directly by code
        if (vm->callStack.top > 1)
        {
            cf = melM_stackOffset(&vm->callStack, 1);
        }
    }

    struct StrFormat sfFmt;
    memset(&sfFmt, 0, sizeof(struct StrFormat));

    va_list args;
    va_start(args, format);
    melStringFmtVargsUtils(&sfFmt, format, args);
    va_end(args);

    melPrintErrorAtSourceUtils(
        vm, 
        "Runtime", 
        sfFmt.buffer, 
        cf != NULL ? cf->function->debug.file : NULL,
        cf != NULL ? cf->function->debug.source : NULL,
        cf != NULL ? cf->function->debug.sourceSize : 0,
        cf != NULL ? (cf->function->debug.count > cf->pc ? cf->function->debug.lines[cf->pc] : 0) : 0,
        1,
        50,
        50
    );

    if (cf != NULL)
    {
        melPrintStackUtils(vm);
        melPrintCallStackUtils(vm);
        melPrintUpvaluesUtils(vm);
    }

    melStringFmtFreeUtils(&sfFmt);

    assert(0);
}

void melPrintVM(VM* vm, struct StrFormat* sf)
{
    if (vm->config.printFn != NULL)
    {
        vm->config.printFn(sf, vm->config.printCtx);
    }
}

TRet melCreateGlobalSymbolKey(VM* vm, const char* k, Value* val)
{
    GCItem* label = melNewString(vm, k, strlen(k));
    // Prevent GCing
    melM_vstackPushGCItem(&vm->stack, label);

    val->type = MELON_TYPE_SYMBOL;
    val->pack.obj = melNewSymbol(vm, label);

    melAddRootValueGC(vm, &vm->gc, val);
    melM_stackPop(&vm->stack);

    return 0;
}