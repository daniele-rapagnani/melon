a = Array.new
c = 0

for j in 0...10
    for i in 0...500000
        a << 1
    end
end

for j in 0...10
    for i in 0...500000
        c = c + a[i]
    end
end

puts(c)