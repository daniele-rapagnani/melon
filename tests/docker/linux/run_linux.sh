#!/bin/bash

docker build . --tag melon/linux-ci:v1
docker run  -ti --mount type=bind,source="$PWD/../../../",target=/opt/melon/ --rm melon/linux-ci:v1 bash
