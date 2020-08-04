a = []
c = 0

for j in range(0, 10):
    for i in range(0, 500000):
        a.append(1)

for j in range(0, 10):
    for i in range(0, 500000):
        c = c + a[i]

print(c)