class A
    def test
        return 1
    end
end

class B < A
end

c = 0
b = B.new

for i in 0...5000000
    c = c + b.test
end

puts(c)