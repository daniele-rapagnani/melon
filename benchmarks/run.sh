
if ! which -s gnuplot; then
    echo "You need gnuplot installed" 1>&2
    exit 1
fi

GIT_HEAD=$(git rev-parse --short HEAD)

GNUPLOT_TEMPL="benchmark.p.template"
GNUPLOT_SCRIPT="benchmark_${GIT_HEAD}.p"

rm -f "$GNUPLOT_SCRIPT"

cat "$GNUPLOT_TEMPL" > "$GNUPLOT_SCRIPT"

LIST="lua melon ruby python"

if [ -n "$1" ]; then
    LIST=$@
fi

for dir in $LIST; do
    if [ -d "$dir" ]; then
        echo ""
        echo "                *** $dir Benchmarks ***"
        echo ""
        cd "$dir" && ./run.sh; cd ..

        printf "'%s' using 3:xticlabels(1) title '%s', " "$dir/results_${GIT_HEAD}.csv" "$dir" >> "$GNUPLOT_SCRIPT"
    fi
done

if gnuplot "$GNUPLOT_SCRIPT" > "benchmark_${GIT_HEAD}.png"; then
    rm -f "$GNUPLOT_SCRIPT"
fi
