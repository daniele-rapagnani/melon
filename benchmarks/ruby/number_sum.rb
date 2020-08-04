c = 0.0;

for j in 0...10
    for i in 0...1000000
        c = c + 0.0001;
    end
end

puts(c);