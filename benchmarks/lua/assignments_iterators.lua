local c = 0

for j=1,10 do
    for i=1,1000000 do
        local b = 1
        c = c + b
    end
end

print(c)