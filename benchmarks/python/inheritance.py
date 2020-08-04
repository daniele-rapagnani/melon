class A:
    def __init__(self):
        self.test = 1

class B(A):
    pass

c = 0
b = B()

for i in range(0, 5000000):
    c = c + b.test

print(c)