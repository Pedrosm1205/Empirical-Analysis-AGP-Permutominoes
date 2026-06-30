# Data collection
import os
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import linregress

guards_vertices = {}

freq_changes = {}
freq_changes_type1 = {}
freq_changes_type2 = {}
freq_changes_vertices = {}
freq_changes_vertices_type1 = {}
freq_changes_vertices_type2 = {}

seq_with_out_change_freq = {}


for entry in os.scandir("data"):
    if entry.is_dir():
        total_verts = int(entry.name.split("_")[1])

        info_file = os.path.join(entry.path, "info.txt")
        if os.path.exists(info_file):
            with open(info_file,"r") as f:
                prev_guards = -1
                seq = 1
                curr_verts = total_verts
                removal = 0
                for line in f:
                    tokens = line.rstrip().split(" ")
                    if len(tokens) == 5:
                        guards = int(tokens[-1])
                        
                        guards_vertices[curr_verts] = guards_vertices.get(curr_verts,[]) + [guards]

                        if prev_guards != -1:
                            freq_changes[prev_guards-guards] = freq_changes.get(prev_guards-guards,0) + 1
                            freq_changes_vertices[curr_verts] = freq_changes_vertices.get(curr_verts,[]) + [prev_guards-guards]

                            if prev_guards-guards < 0:
                                print(entry.path,curr_verts)

                            if prev_guards == guards: 
                                seq += 1
                            else:
                                seq_with_out_change_freq[seq] = seq_with_out_change_freq.get(seq,0) + 1
                                seq = 1  


                            xs = set()
                            ys = set()
                            curr_poly = []
                            curr_poly_file = os.path.join(entry.path, "removal"+str(removal))
                            if os.path.exists(curr_poly_file):
                                with open(curr_poly_file,"r") as cf:
                                    for cline in cf:
                                        ctokens = cline.rstrip().split(" ")
                                        if len(ctokens) == 2:
                                            curr_poly.append((int(ctokens[0]),int(ctokens[1])))
                                            xs.add(int(ctokens[0]))
                                            ys.add(int(ctokens[1]))

                            for (x,y) in prev_poly:
                                if x not in xs and y in ys:
                                    x_ref,y_ref = x,y 

                            A,B = None,None
                            for (x,y) in curr_poly:
                                if y == y_ref:
                                    if A == None:
                                        A = x
                                    else:
                                        B = x
                                        break

                            if x_ref < min(A,B) or x_ref > max(A,B): # Type I iter
                                #print("type 1")
                                freq_changes_type1[prev_guards-guards] = freq_changes_type1.get(prev_guards-guards,0) + 1
                                freq_changes_vertices_type1[curr_verts] = freq_changes_vertices_type1.get(curr_verts,[]) + [prev_guards-guards]
                                
                            else: # Type II iter
                                #print("type 2")
                                freq_changes_type2[prev_guards-guards] = freq_changes_type2.get(prev_guards-guards,0) + 1
                                freq_changes_vertices_type2[curr_verts] = freq_changes_vertices_type2.get(curr_verts,[]) + [prev_guards-guards]
                                

                            prev_poly = curr_poly

                        else:
                            prev_poly = []
                            prev_poly_file = os.path.join(entry.path, "test")
                            if os.path.exists(prev_poly_file):
                                with open(prev_poly_file,"r") as pf:
                                    for pline in pf:
                                        ptokens = pline.rstrip().split(" ")
                                        if len(ptokens) == 2:
                                            prev_poly.append((int(ptokens[0]),int(ptokens[1])))

                        removal += 1
                        prev_guards = guards
                        curr_verts -= 2


        else:
            print("No info.txt")


def plot_avg_guards(guards_vertices):
    gv_keys  = sorted(guards_vertices.keys())
    gv_means = np.array([np.mean(guards_vertices[k]) for k in gv_keys])
    gv_stds  = np.array([np.std(guards_vertices[k])  for k in gv_keys])

    slope, intercept, r, *_ = linregress(gv_keys, gv_means)
    fit_line = slope * np.array(gv_keys) + intercept

    fig1, ax1 = plt.subplots(figsize=(12, 5))
    ax1.plot(gv_keys, gv_means, color="#4C72B0", linewidth=1.2, label="Mean value")
    ax1.fill_between(gv_keys,
                    gv_means - gv_stds,
                    gv_means + gv_stds,
                    alpha=0.25, color="#4C72B0", label="±1 Std dev")
    
    ax1.text(0.02, 0.95,
            f"Linear fit: slope={slope:.4f}, intercept={intercept:.4f}, R²={r**2:.3f}",
            transform=ax1.transAxes, fontsize=9,
            verticalalignment="top",
            bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.7))
    ax1.set_title("Number of guards needed per number of vertices", fontsize=12)
    ax1.set_xlabel("Number of Vertices")
    ax1.set_ylabel("Optimal number of Guards")
    ax1.legend(fontsize=9)
    ax1.grid(axis="y", linestyle="--", alpha=0.4)
    plt.tight_layout()
    plt.savefig("1_guards_vertices.png", dpi=150, bbox_inches="tight")
    plt.show()
    
def plot_hist_diffs(freq_changes):
    keys = list(freq_changes.keys())
    values = list(freq_changes.values())

    plt.figure()
    plt.bar(keys, values)

    plt.xlabel("Diff")
    plt.ylabel("Count")
    plt.title("Evolution of the number of guards after one iteration of I&P")

    plt.show()


def plot_avg_diff_per_size(freq_changes_vertices):
    bin_size = 750

    fcv_keys  = sorted(freq_changes_vertices.keys())
    fcv_means = {k: np.mean(freq_changes_vertices[k]) for k in fcv_keys}

    min_k, max_k = fcv_keys[0], fcv_keys[-1]
    bins = range(min_k, max_k + bin_size, bin_size)

    bin_centers, bin_means = [], []
    for b in bins:
        keys_in_bin = [k for k in fcv_keys if b <= k < b + bin_size]
        if keys_in_bin:
            bin_centers.append(b + bin_size / 2)          # centre of the bin
            bin_means.append(np.mean([fcv_means[k] for k in keys_in_bin]))

    fig, ax = plt.subplots(figsize=(16, 6))
    ax.set_ylim(bottom=0)
    ax.plot(bin_centers, bin_means, marker="o", linewidth=1.4)
    ax.set_xlabel("Number of Vertices (grouped by 750)")
    ax.set_ylabel("Mean number of guards change")
    ax.set_xticks(bin_centers)
    ax.set_xticklabels([f"{int(c - bin_size/2)}–{int(c + bin_size/2)}" for c in bin_centers],
                        rotation=45, ha="right", fontsize=8)
    ax.grid(axis="y", linestyle="--", alpha=0.4)
    plt.tight_layout()
    plt.show()

def seqs_size(seq_with_out_change_freq):
    keys = list(seq_with_out_change_freq.keys())
    values = list(seq_with_out_change_freq.values())

    plt.figure()
    plt.bar(keys, values)

    plt.xlabel("Length of the sequence")
    plt.ylabel("Frequency")
    plt.title("Sequence where the optimal number of guards does not change")

    plt.show()


def plot_hist_diffs_compare(freq_changes_1, freq_changes_2,
                             label1, label2):
    all_keys = sorted(set(freq_changes_1.keys()) | set(freq_changes_2.keys()))

    values1 = [freq_changes_1.get(k, 0) for k in all_keys]
    values2 = [freq_changes_2.get(k, 0) for k in all_keys]

    fig, axes = plt.subplots(1, 2, figsize=(16, 6), sharey=True)

    for ax, values, label, color in zip(axes, [values1, values2], [label1, label2], ["C0", "C1"]):
        ax.bar(all_keys, values, color=color)
        ax.set_xlabel("Diff")
        ax.set_title(label)
        ax.set_xticks(all_keys)

    axes[0].set_ylabel("Count")
    fig.suptitle("Evolution of the number of guards after one iteration of I&P by type")
    plt.tight_layout()
    plt.show()


def plot_avg_diff_per_size_compare(freq_changes_vertices_1, freq_changes_vertices_2,
                                    label1, label2, bin_size=750):
    def compute_bins(freq_changes_vertices, min_k, max_k):
        fcv_keys = sorted(freq_changes_vertices.keys())
        fcv_means = {k: np.mean(freq_changes_vertices[k]) for k in fcv_keys}
        bins = range(min_k, max_k + bin_size, bin_size)
        bin_centers, bin_means = [], []
        for b in bins:
            keys_in_bin = [k for k in fcv_keys if b <= k < b + bin_size]
            if keys_in_bin:
                bin_centers.append(b + bin_size / 2)
                bin_means.append(np.mean([fcv_means[k] for k in keys_in_bin]))
        return bin_centers, bin_means

    all_keys = list(freq_changes_vertices_1.keys()) + list(freq_changes_vertices_2.keys())
    min_k, max_k = min(all_keys), max(all_keys)

    centers1, means1 = compute_bins(freq_changes_vertices_1, min_k, max_k)
    centers2, means2 = compute_bins(freq_changes_vertices_2, min_k, max_k)

    fig, axes = plt.subplots(1, 2, figsize=(20, 6), sharey=True)

    for ax, centers, means, label, color in zip(
        axes, [centers1, centers2], [means1, means2], [label1, label2], ["C0", "C1"]
    ):
        ax.set_ylim(bottom=0)
        ax.plot(centers, means, marker="o", linewidth=1.4, color=color)
        ax.set_title(label)
        ax.set_xlabel(f"Number of Vertices (grouped by {bin_size})")
        ax.set_xticks(centers)
        ax.set_xticklabels(
            [f"{int(c - bin_size/2)}–{int(c + bin_size/2)}" for c in centers],
            rotation=45, ha="right", fontsize=8
        )
        ax.grid(axis="y", linestyle="--", alpha=0.4)

    axes[0].set_ylabel("Mean number of guards change")
    plt.tight_layout()
    plt.show()


plot_avg_guards(guards_vertices)
plot_hist_diffs(freq_changes)
plot_avg_diff_per_size(freq_changes_vertices)
seqs_size(seq_with_out_change_freq)

plot_hist_diffs_compare(freq_changes_type1,freq_changes_type2,"Type I","Type II")
plot_avg_diff_per_size_compare(freq_changes_vertices_type1,freq_changes_vertices_type2,"Type I","Type II")
