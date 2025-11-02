import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '../..'))

import chart_tools
import matplotlib.pyplot as plt


if __name__ == "__main__":
    args = chart_tools.parse_cli()

    fig, ax = plt.subplots(1, figsize=(15, 15))
    chart_tools.setup_axes(fig, ax)

    X, Y = [], []
    for size in [100_000, 1_000_000, 5_000_000, 10_000_000]:
        print(f"Start collecting time for array with size={size}")
            # n_values = [2, 8, 14, 20, 26, 32, 38, 44, 50, 56, 62]
        X, Y = chart_tools.collect_time_stats(
            f"{args.argv} {size}",
            args.averaging_parameter,
            n_values = list(range(2, 64 + 1))
        )
        chart_tools.plot_chart(ax, X, Y, label=f"size={size}")

    # Use last coordinates to plot acceleration chart
    X_ac, Y_ac = X[1:], []
    for i in range(1, len(X)):
        Y_ac.append(Y[i] - Y[i-1])
    fig_ac, ax_ac = plt.subplots(1, figsize=(10, 5))
    chart_tools.setup_axes(fig_ac, ax_ac, "Acceleration")
    chart_tools.plot_chart(ax_ac, X_ac, Y_ac)
    fig_ac.legend(fontsize=12)
    fig_ac.savefig("acceleration.png", dpi=300, bbox_inches="tight", facecolor="white")

    fig.legend(fontsize=12)
    fig.savefig(args.output, dpi=300, bbox_inches="tight", facecolor="white")
