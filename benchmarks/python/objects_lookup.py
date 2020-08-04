o = {}
o[500000] = 1

for i in range(0, 5000000):
    if i in o:
        print(i)