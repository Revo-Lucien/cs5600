#!/usr/bin/env python3
"""
Reproduces the "unfairness vs. job length" study from OSTEP Chapter 9
(Proportional-Share Scheduling), and compares lottery scheduling against
stride scheduling.

Two jobs, each with an equal number of tickets (so the "fair" outcome is
for both to finish at the same time). We vary the total job length (in
quanta) and measure the unfairness metric:

    U = time first job finishes / time second job finishes

U = 1 means perfectly fair (both finished at the same time).
U > 1 means one job finished much earlier than the other (unfair).

For lottery scheduling we average U over many random trials per job
length, since the outcome is probabilistic. Stride scheduling is
deterministic, so one trial per length suffices.
"""

import random
import matplotlib.pyplot as plt

def run_lottery(length, tickets=(100, 100), seed=None):
    rng = random.Random(seed)
    remaining = [length, length]
    finish = [None, None]
    clock = 0
    active = [0, 1]
    while active:
        active_total = sum(tickets[j] for j in active)
        winner_ticket = rng.randrange(active_total)
        cum = 0
        winner = None
        for j in active:
            cum += tickets[j]
            if winner_ticket < cum:
                winner = j
                break
        remaining[winner] -= 1
        clock += 1
        if remaining[winner] == 0:
            finish[winner] = clock
            active.remove(winner)
    return finish

def run_stride(length, tickets=(100, 100)):
    BIG = 10_000_000
    stride = [BIG // t for t in tickets]
    pass_ = [0, 0]
    remaining = [length, length]
    finish = [None, None]
    clock = 0
    active = [0, 1]
    while active:
        # pick the job among the active ones with the lowest pass value
        winner = min(active, key=lambda j: pass_[j])
        remaining[winner] -= 1
        pass_[winner] += stride[winner]
        clock += 1
        if remaining[winner] == 0:
            finish[winner] = clock
            active.remove(winner)
    return finish

def unfairness(finish):
    t1, t2 = finish
    return max(t1, t2) / min(t1, t2)

# job lengths to test, log-spaced like the textbook figure
lengths = [1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000]
TRIALS = 200  # per length, for the lottery scheduler (random, needs averaging)

lottery_avg = []
stride_avg = []

for L in lengths:
    # Lottery: average over many random seeds
    us = [unfairness(run_lottery(L, seed=trial)) for trial in range(TRIALS)]
    lottery_avg.append(sum(us) / len(us))

    # Stride: deterministic, always the same result for a given length
    stride_avg.append(unfairness(run_stride(L)))

# ---- plot ----
plt.figure(figsize=(7, 5))
plt.plot(lengths, lottery_avg, marker='o', label='Lottery Scheduling (avg of %d trials)' % TRIALS)
plt.plot(lengths, stride_avg, marker='s', label='Stride Scheduling (deterministic)')
plt.xscale('log')
plt.xlabel('Job Length (total time slices)')
plt.ylabel('Unfairness (U)')
plt.title('Unfairness vs. Job Length: Lottery vs. Stride Scheduling\n(two jobs, equal tickets, quantum = 1)')
plt.axhline(y=1, color='gray', linestyle=':', linewidth=1)
plt.legend()
plt.grid(True, which='both', linestyle='--', alpha=0.4)
plt.tight_layout()
plt.savefig('/mnt/user-data/outputs/fairness_study.png', dpi=150)
print("Saved chart.")

print("\nlength  lottery_U   stride_U")
for L, lu, su in zip(lengths, lottery_avg, stride_avg):
    print(f"{L:7d} {lu:10.3f} {su:10.3f}")
