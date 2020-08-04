local c = 0
local j = 0
local i = 0

while j < 10 do
    i = 0
    while i < 1000000 do
        local b = 1
        c = c + b
        i = i + 1
    end

    j = j + 1
end

print(c)