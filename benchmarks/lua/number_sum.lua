local c = 0.0

for j=1,10 do
    for i=1,1000000 do
        c = c + 0.0001
    end
end

print(c)