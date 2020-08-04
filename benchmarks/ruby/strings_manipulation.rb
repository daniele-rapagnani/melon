b = nil 

for i in 0...1000000
    a = "Lorem ipsum dolor sit amet lorem ipsum dolor sit amet"
    b = a.gsub("dolor", "DOLOR") + a.index("sit").to_s
end

puts(b)
