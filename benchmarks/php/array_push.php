<?php

$c = [];

for($j = 0; $j < 10; $j++) {
    for($i = 0; $i < 1000000; $i++) {
        array_push($c, 1);
    }
}

echo count($c);