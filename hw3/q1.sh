#!/usr/bin/env bash
#
# q1.sh - Chapter 8, Question 1
#
# "Run a few randomly-generated problems with just two jobs and two
#  queues; compute the MLFQ execution trace for each. Make your life
#  easier by limiting the length of each job and turning off I/Os."
#
# Takes no arguments. Loops over a handful of seeds and, for each one,
# asks mlfq.py for a random 2-job / 2-queue problem with job length
# capped at 20 and I/O disabled (-M 0), then solves it (-c).
#
# Run with: ./q1.sh

MLFQ="./mlfq.py"
SEEDS=(0 1 2 3 4)

for seed in "${SEEDS[@]}"; do
    echo "=============================================="
    echo " Random problem - seed $seed"
    echo " Command: $MLFQ -s $seed -n 2 -j 2 -m 20 -M 0 -c"
    echo "=============================================="
    python3 "$MLFQ" -s "$seed" -n 2 -j 2 -m 20 -M 0 -c
    echo
done
