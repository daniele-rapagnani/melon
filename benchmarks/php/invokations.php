<?php

$c = 0;

$f = function() {
    return 1;
};

for($j = 0; $j < 10; $j++) {
    for ($i = 0; $i < 1000000; $i++) {
        $c = $c + $f();
    }
}

echo "$c\n";