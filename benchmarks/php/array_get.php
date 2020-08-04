<?php

$a = [];
$c = 0;

for($j = 0; $j < 10; $j++) {
    for($i = 0; $i < 500000; $i++) {
        array_push($a, 1);
    }
}

for($j = 0; $j < 10; $j++) {
    for($i = 0; $i < 500000; $i++) {
        $c = $c + $a[i];
    }
}

echo $c;