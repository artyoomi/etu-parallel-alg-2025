import chart_tools
import matplotlib.pyplot as plt

if __name__ == "__main__":
    args = chart_tools.parse_cli()

    fig, ax = plt.subplots(1, figsize=(10, 10))
    chart_tools.setup_axes(fig, ax)

    fig_ac, ax_ac = plt.subplots(1, figsize=(10, 5))
    chart_tools.setup_axes(fig_ac, ax_ac)

    fig_ac_seq, ax_ac_seq = plt.subplots(1, figsize=(10, 5))
    chart_tools.setup_axes(fig_ac_seq, ax_ac_seq)

    for i in range(100, 1_000, 200):
        X_seq, Y_seq = chart_tools.collect_time_stats(
            f"./sequential_task {i} {i}",
            args.averaging_parameter,
            n_values=(1,)
        )
        X, Y = chart_tools.collect_time_stats(
            f"./task {i} {i}",
            args.averaging_parameter,
            n_values=(list(range(1, 48 + 1, 3)))
        )
        chart_tools.plot_chart(ax, X, Y, label=f"Matrix size equal to {i}")

        X_ac, Y_ac = chart_tools.calculate_acceleration(X, Y)
        chart_tools.plot_chart(ax_ac, X_ac, Y_ac)

        X_ac_seq, Y_ac_seq = X, [Y[i] - Y_seq[0] for i in range(len(Y))]
        chart_tools.plot_chart(ax_ac_seq, X_ac_seq, Y_ac_seq, label=f"Matrix size equal to {i}")

    fig.legend(fontsize=12)
    fig.savefig("chart.png", dpi=300, bbox_inches="tight", facecolor="white")
    fig_ac.legend(fontsize=12)
    fig_ac.savefig("acceleration.png", dpi=300, bbox_inches="tight", facecolor="white")
    fig_ac_seq.legend(fontsize=12)
    fig_ac_seq.savefig("acceleration_seq.png", dpi=300, bbox_inches="tight", facecolor="white")
