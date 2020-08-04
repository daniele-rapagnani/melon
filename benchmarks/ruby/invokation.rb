
c = 0
f = lambda { return 1 }

(1..10).each {
    (1..1000000).each {
        c = c + f.call
    }
}

puts c