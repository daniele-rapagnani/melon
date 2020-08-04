local b = nil

for i=1,1000000 do
    local a = "Lorem ipsum dolor sit amet lorem ipsum dolor sit amet"
    b = a:gsub("dolor", "DOLOR") .. a:find("sit")
end

print(b)