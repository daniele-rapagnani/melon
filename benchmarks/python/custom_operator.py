class Vector:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y)

v1 = Vector(0.25, 0.5)
v2 = Vector(0.0, 0.0)

for i in range(0, 500000):
    v2 = v1 + v2

print(v2.x)
print(v2.y)