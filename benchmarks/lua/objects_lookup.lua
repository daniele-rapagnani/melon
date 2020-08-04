local o = {};
o[500000] = 1;

for i=0,5000000 do
    if o[i] ~= nil then
        print(i);
    end
end