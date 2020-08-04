c = {}
j = 0
i = 0

while(j < 10)
    i = 0
    while (i < 500000)
        c[i] = 1
        i = i + 1
    end

    j = j + 1
end

j = 0
i = 0
c['total'] = 0

while(j < 10)
    i = 0
    while (i < 500000)
        c['total'] = c['total'] + c[i]
        i = i + 1
    end

    j = j + 1
end

puts(c['total'])