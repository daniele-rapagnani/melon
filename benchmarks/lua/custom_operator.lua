
local Vector = {}

function Vector.create(x, y)
    local v = { x = x, y = y }
    setmetatable(v, Vector)
    return v
end

function Vector.__add(lhs, rhs)
    return Vector.create(lhs.x + rhs.x, lhs.y + rhs.y)
end

local v1 = Vector.create(0.25, 0.5)
local v2 = Vector.create(0, 0)

for i=1,500000 do
    v2 = v1 + v2
end

print(v2.x)
print(v2.y)