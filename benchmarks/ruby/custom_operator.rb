class Vector
    def initialize(x, y)
        @x = x
        @y = y
    end

    def +(other)
        return Vector.new(@x + other.x, @y + other.y)
    end

    def x()
        return @x
    end

    def y()
        return @y
    end
end

v1 = Vector.new 0.25, 0.5
v2 = Vector.new 0.0, 0.0

for i in 0...500000
    v2 = v1 + v2
end

puts(v2.x)
puts(v2.y)