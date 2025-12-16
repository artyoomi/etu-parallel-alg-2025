import chart_tools
import matplotlib.pyplot as plt

if __name__ == "__main__":
    args = chart_tools.parse_cli()

    fig, ax = plt.subplots(1, figsize=(10, 10))
    chart_tools.setup_axes(fig, ax)

    X, Y = [], []
    for i in range(100, 2_000, 100):
        _, loc_Y = chart_tools.collect_time_stats(
            f"{args.argv} {i} {i}",
            args.averaging_parameter,
            n_values=[1]
        )
        X.append(i)
        Y.append(loc_Y[0])

    chart_tools.plot_chart(ax, X, Y)
    fig.legend(fontsize=12)
    fig.savefig("chart.png", dpi=300, bbox_inches="tight", facecolor="white")

