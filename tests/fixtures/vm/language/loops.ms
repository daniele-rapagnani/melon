
let a = 10;

while (a >= 0)
{
    io.print("Iteration: " .. string.toString(a));
    a = a - 1;
}

while (a > 1)
{
    io.print("Don't print this");
}

let b = 10;
let keepGoing = true;

while (b >= 0 && keepGoing)
{
    io.print("Keep going!");

    if (b == 6)
    {
        keepGoing = false;
    }

    b = b - 1;
}

for (let i in 0..5)
{
   io.print("For iteration: " .. string.toString(i));
}

// Break/Continue

a = 0;
while (true)
{
    if (a >= 10)
    {
        break;
    }

    io.print("Not breaking: " .. string.toString(a));
    a = a + 1;
}
b = 0;

while (true)
{
    a = 0;

    while (true)
    {
        if (a >= 10)
        {
            break;
        }

        io.print("Not breaking inner: " .. string.toString(a));
        a = a + 1;
    }

    if (b >= 3)
    {
        break;
    }

    io.print("Not breaking outer: " .. string.toString(b));
    b = b + 1;
}

a = 0;
while (a < 10)
{
    a = a + 1;

    if (a % 2 == 0)
    {
        continue;
    }

    io.print("Continue passed: " .. string.toString(a));
}

b = 0;
while (b < 5)
{
    b = b + 1;

    if (b % 2 == 1)
    {
        continue;
    }

    io.print("Continue outer passed: " .. string.toString(b));

    a = 0;

    while (a < 10)
    {
        a = a + 1;

        if (a % 2 == 0)
        {
            continue;
        }

        io.print("Continue inner passed: " .. string.toString(a));
    }
}

a = 0;
while (true)
{
    a = a + 1;

    if (a % 2 == 0)
    {
        continue;
    }

    if (a >= 5)
    {
        break;
    }

    io.print("Running: " .. string.toString(a));
}

for (let i1 in 0..10)
{
    if (i1 % 2 == 0)
    {
        continue;
    }

    io.print("For value: " .. string.toString(i1));

    if (i1 == 5)
    {
        break;
    }
}

let arr = [1, 5, 7, 9, 10, 11];

for (let i2 in arr)
{
    if (i2 % 3 == 0)
    {
        continue;
    }

    io.print("For value: " .. string.toString(i2));

    if (i2 == 10)
    {
        break;
    }
}