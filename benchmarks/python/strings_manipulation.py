b = None

for i in range(0,1000000):
    a = "Lorem ipsum dolor sit amet lorem ipsum dolor sit amet"
    b = a.replace("dolor", "DOLOR") + str(a.find("sit"))

print(b)