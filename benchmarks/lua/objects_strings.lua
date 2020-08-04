local c = {}
local j = 0
local i = 0

while j < 10 do
    i = 0
    while i < 100000 do
        c["key" .. i] = 1
        i = i + 1
    end

    j = j + 1
end

j = 0
i = 0
c.total = 0

while j < 10 do
    i = 0
    while i < 100000 do
        c.total = c.total + c["key" .. i];
        i = i + 1
    end

    j = j + 1
end

print(c.total)