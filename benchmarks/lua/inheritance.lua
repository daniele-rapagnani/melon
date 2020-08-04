local a = {
    test = 1
}

local b = {}
setmetatable(b, {
    __index = a
})

local c = 0

for i=1,5000000 do
    c = c + b.test;
end

print(c)