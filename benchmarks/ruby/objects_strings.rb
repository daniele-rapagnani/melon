c = {}
j = 0
i = 0

while(j < 10)
    i = 0
    while (i < 100000)
        c['key' + i.to_s] = 1
        i = i + 1
    end

    j = j + 1
end

j = 0
i = 0
c['total'] = 0

while(j < 10)
    i = 0
    while (i < 100000)
        c['total'] = c['total'] + c['key' + i.to_s]
        i = i + 1
    end

    j = j + 1
end

puts(c['total'])