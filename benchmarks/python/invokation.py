c = 0
j = 0
i = 0

f = lambda : 1

while(j < 10):
    i = 0

    while (i < 1000000):
        c = c + f()
        i = i + 1

    j = j + 1

print(c)