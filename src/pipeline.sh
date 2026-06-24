#!/usr/bin/env bash
set -euo pipefail

# Usage: ./run_pipeline.sh <arg1> <arg2>
# Example: ./run_pipeline.sh 1 14

ARG1="${1:?Usage: $0 <arg1> <arg2>}"
ARG2="${2:?Usage: $0 <arg1> <arg2>}"

# --- Determine next data[N] index ---
mkdir -p data
next_idx=0
for d in data/data[0-9]*_[0-9]*/; do
    [[ -d "$d" ]] || continue
    idx="${d#data/data}"
    idx="${idx%%_*}"
    [[ "$idx" =~ ^[0-9]+$ ]] || continue
    (( idx + 1 > next_idx )) && next_idx=$(( idx + 1 ))
done
OUTDIR="data/data${next_idx}_${ARG2}"
mkdir -p "$OUTDIR"
# echo "Output directory: $OUTDIR"

# echo "[1] Running swerc_inflate_cut..."
echo "$ARG1 $ARG2" | ./swerc_inflate_cut > "$OUTDIR/test"

# echo "[2] Running gridnew..."
./gridnew < "$OUTDIR/test" > "$OUTDIR/test_grid"

if [ $ARG2 -lt 500 ]; then 
  # echo "[3] Running draw_orthopol for test.tex..."
  ./draw_orthopol < "$OUTDIR/test" > "$OUTDIR/test.tex"
fi
  
if [ $ARG2 -lt 500 ]; then 
  # echo "[4] Running pdflatex on test.tex..."
  pdflatex -interaction=nonstopmode -output-directory="$OUTDIR" "$OUTDIR/test.tex" > /dev/null
fi

# echo "[5] Running EXE..."
(cd build && ./EXE < "../$OUTDIR/test_grid" > "../$OUTDIR/info.txt")

# echo "[6-7] Processing removal* files..."
pids=()
for f in build/removal[0-9]*; do
    [[ -f "$f" ]] || continue
    base="$(basename "$f")"
    (
        echo "  -> $base"
        
        if [ $ARG2 -lt 500 ]; then 
          ./draw_orthopol < "$f" > "$OUTDIR/${base}.tex"
        fi
        
        mv "$f" "$OUTDIR"

        if [ $ARG2 -lt 500 ]; then 
          pdflatex -interaction=nonstopmode -output-directory="$OUTDIR" "$OUTDIR/${base}.tex" > /dev/null
        fi
    ) &
    pids+=($!)
done

for pid in "${pids[@]}"; do
    wait "$pid" || { echo "ERROR: a removal job failed (pid $pid)"; exit 1; }
done

echo "Done. All output in $OUTDIR"
