c = 0

for j in 0...10
    for i in 0...1000000
        b = 1
        c = c + b
    end
end

puts(c)