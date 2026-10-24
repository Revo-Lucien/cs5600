#!/usr/bin/env python3
# usage: python3 plot_tlb_fixed.py [results.txt] [output.png]
# Accepts lines like "8 pages: 2.50 ns/access" or "8 2.50".

import re; import sys; import matplotlib
matplotlib.use("Agg")  # no display needed; saves to PNG
import matplotlib.pyplot as plt

infile = sys.argv[1] if len(sys.argv) > 1 else "results.txt"
outfile = sys.argv[2] if len(sys.argv) > 2 else "tlb.png"

pages, times = [], []
with open(infile) as f:
    for line in f:
        nums = re.findall(r"\d+\.?\d*", line)
        if len(nums) >= 2:
            pages.append(int(nums[0]))
            times.append(float(nums[1]))

if not pages:
    sys.exit(f"no data parsed from {infile}")

fig, ax = plt.subplots(figsize=(7, 4.5))
ax.plot(pages, times, marker="o", color="tab:blue")

ax.set_xscale("log", base=2)
ax.set_xticks(pages)
ax.set_xticklabels([str(p) for p in pages], rotation=45)
ax.set_xlabel("Number Of Pages")
ax.set_ylabel("Time Per Access (ns)")
ax.set_title("TLB Size Measurement")
ax.set_ylim(bottom=0)
ax.grid(True, alpha=0.3)

fig.tight_layout()
fig.savefig(outfile, dpi=150)
print(f"saved {outfile} ({len(pages)} points)")
