c = {}
j = 0
i = 0

while(j < 10):
    i = 0
    while (i < 100000):
        c['key' + str(i)] = 1
        i = i + 1

    j = j + 1

j = 0
i = 0
c['total'] = 0

while(j < 10):
    i = 0
    while (i < 100000):
        c['total'] = c['total'] + c['key' + str(i)]
        i = i + 1

    j = j + 1

print(c['total'])