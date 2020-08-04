
function run_tests {
    TEST_RESULTS="TEST NAME,TEST NUMBER,TIME,RESULT,CMD"
    TEST_NUMBER=1

    for f in *.${1}; do
        if [ ! "${f:0:1}" = "_" ]; then
            TEST_NAME=$(echo "${f%.*}" | tr '_' ' ')

            echo "==========================="
            echo "$TEST_NAME"
            echo ""
            TIME=$({ time -p ${@:2} "$f" | tee cmd.output; } 2>&1)
            echo "$TIME"
            echo "==========================="
            echo ""


            TIME_REAL=$(echo "$TIME" | grep -Eo '^real\s+\d+\.\d+$' | grep -Eo '[0-9\.]+')
            TEST_RESULTS+=$(printf "\n\"%s\",%s,%s,\"%s\",\"%s\"" "$TEST_NAME" "$TEST_NUMBER" "$TIME_REAL" "$(cat cmd.output | tr "\n" " " | sed 's/ *$//g')" "${*:2}")

            rm -f cmd.output;

            TEST_NUMBER=$((TEST_NUMBER + 1))
        fi
    done

    GIT_HEAD=$(git rev-parse --short HEAD)

    echo "$TEST_RESULTS" > "results_${GIT_HEAD}.csv"
}