local c = 0

function f()
    return 1
end

for j=1,10 do
    for i=1,1000000 do
        c = c + f()
    end
end

print(c)