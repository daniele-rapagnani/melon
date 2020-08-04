local a = {}
local c = 0

for j=1,10 do
    for i=1,500000 do
        table.insert(a, 1)
    end
end

for j=1,10 do
    for i=1,500000 do
        c = c + a[i]
    end
end

print(c)