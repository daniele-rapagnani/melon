o = {};
o[500000] = 1;

for i in 0..5000000
    if o.key?(i)
        puts(i);
    end
end