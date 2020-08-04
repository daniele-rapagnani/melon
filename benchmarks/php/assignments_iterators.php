<?php

$c = 0;

for($j = 0; $j < 10; $j++) {
    for ($i = 0; $i < 1000000; $i++) {
        $b = 1;
        $c = $c + $b;
    }
}

echo "$c\n";