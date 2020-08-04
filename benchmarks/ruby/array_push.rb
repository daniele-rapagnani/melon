c = Array.new;

for j in 0...10
    for i in 0...1000000
        c << i;
    end
end

puts(c.size);