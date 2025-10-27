import argparse
import subprocess

import matplotlib.pyplot as plt

from functools import partial

def collect_time_stats(exe: str, avg_param: int, n_values: list = [1, 2, 4, 8, 16, 32, 64]) -> dict:
    time_stats = dict()

    for n in n_values:
        time_values = []
        for i in range(avg_param):
            print(f"{i+1:3}) n={n}")

            result = subprocess.run(
                f"mpirun --oversubscribe -n {n} {exe}".split(),
                encoding="utf-8",
                capture_output=True
            )
            if result.returncode != 0:
                print("Something went wrong in subprocess run, exiting...")
                exit(1)

            time_values.append(float(result.stdout))

        if len(time_values) == 0:
            print("Empty time values list, exiting...")
            exit(1)
        time_stats[n] = sum(time_values) / len(time_values)

    return time_stats

def int_limited(arg: str, lower: int = None, upper: int = None):
    try:
        limited_int_arg = int(arg)
        if lower is not None and limited_int_arg < lower:
            raise argparse.ArgumentTypeError(f"Int value must be greater than {lower}")
        if upper is not None and limited_int_arg > upper:
            raise argparse.ArgumentTypeError(f"Int value must be lower than {upper}")

        return limited_int_arg
    except ValueError:
        raise argparse.ArgumentTypeError(f"Invalid int value: {arg}")

int_is_positive = partial(int_limited, lower=1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot chart for MPI program")
    parser.add_argument(
        "-p",
        "--averaging-parameter",
        type=int_is_positive,
        required=True,
        help="The number of values for which the averaging is performed"
    )
    parser.add_argument(
        "-e",
        "--executable",
        type=str,
        required=True,
        help="Name of executable to use"
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        default="chart.png",
        help="Chart image filename"
    )

    args = parser.parse_args()

    X, Y = zip(*collect_time_stats(args.executable, args.averaging_parameter).items())

    fig, ax = plt.subplots(1, figsize=(15, 15))
    fig.suptitle("Graph of the correspondence of the number of parallel processes to the time")

    ax.grid(True, linestyle='--', alpha=0.8)

    # Setting up x axis
    ax.set_xlabel('n', fontsize=12)

    # Setting up y axis
    ax.set_ylabel('Elapsed time, in seconds', fontsize=12)

    ax.plot(X, Y)
    # Add this after the ax.plot(X, Y) line
    for i, (x, y) in enumerate(zip(X, Y)):
        ax.annotate(f'{y}', 
                    (x, y),
                    textcoords="offset points",
                    xytext=(0,10),
                    ha='center',
                    fontsize=12,
                    alpha=0.7)

    fig.legend(fontsize=12)
    fig.savefig(args.output, dpi=300, bbox_inches="tight", facecolor="white")
