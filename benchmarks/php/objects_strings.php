<?php

$c = [];
$j = 0;
$i = 0;

while($j < 10) {
    $i = 0;
    while ($i < 100000) {
        $c["key" . $i] = 1;
        $i = $i + 1;
    }

    $j = $j + 1;
}

$j = 0;
$i = 0;

while($j < 10) {
    $i = 0;
    while ($i < 100000) {
        $c['total'] += $c["key" . $i];
        $i = $i + 1;
    }

    $j = $j + 1;
}

echo $c['total'] . "\n";