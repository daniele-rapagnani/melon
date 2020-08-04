<?php
$o = [];
$o[500000] = 1;

for ($i = 0; $i < 5000000; $i++) {
    if (array_key_exists($i, $o)) {
        echo $i;
    }
}