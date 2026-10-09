#!/usr/bin/env bash
#
# q2.sh - Chapter 8, Question 2
#
# "How would you run the scheduler to reproduce each of the examples
#  in the chapter?"
#
# Takes no arguments. Loops over the 3 examples from the chapter and
# calls mlfq.py once per example with the parameters that reproduce it.
#
# Run with: ./q2.sh

MLFQ="./mlfq.py"

# Each entry is "description|args..."
EXAMPLES=(
    "Example 1 (Figure 8.2) - A Single Long-Running Job|-n 3 -q 10 -l 0,200,0 -c"
    "Example 2 (Figure 8.3) - Along Came A Short Job|-n 3 -q 10 -l 0,180,0:100,20,0 -c"
    "Example 3 (Figure 8.4) - What About I/O?|-n 3 -q 10 -l 0,175,0:50,25,1 -i 5 -S -c"
)

for entry in "${EXAMPLES[@]}"; do
    desc="${entry%%|*}"
    args="${entry#*|}"
    echo "=============================================="
    echo " $desc"
    echo " Command: $MLFQ $args"
    echo "=============================================="
    python3 "$MLFQ" $args
    echo
done
