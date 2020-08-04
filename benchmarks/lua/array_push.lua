local c = {}

for j=1,10 do
    for i=1,1000000 do
        table.insert(c, i)
    end
end

print(#c)