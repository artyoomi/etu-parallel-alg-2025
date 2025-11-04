import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '../..'))

import chart_tools
import matplotlib.pyplot as plt

if __name__ == "__main__":
    args = chart_tools.parse_cli()

    fig, ax = plt.subplots(1, figsize=(10, 10))
    chart_tools.setup_axes(fig, ax)

    X, Y = chart_tools.collect_time_stats(
        f"{args.argv}",
        args.averaging_parameter,
        n_values=(list(range(2, 32 + 1)))
    )
    chart_tools.plot_chart(ax, X, Y)
    fig.legend(fontsize=12)
    fig.savefig("chart.png", dpi=300, bbox_inches="tight", facecolor="white")

    fig_ac, ax_ac = plt.subplots(1, figsize=(10, 5))
    chart_tools.setup_axes(fig_ac, ax_ac)
    X_ac, Y_ac = chart_tools.calculate_acceleration(X, Y)
    chart_tools.plot_chart(ax_ac, X_ac, Y_ac)
    fig_ac.legend(fontsize=12)
    fig_ac.savefig("acceleration.png", dpi=300, bbox_inches="tight", facecolor="white")

