import sys
import subprocess

import matplotlib.pyplot as plt

def measure_time(program_name: str, iter: int = 15) -> dict:
    """Measure time for different amount of processes for given program
    
    :param program_name: name of program to execute
    :param iter: amount of iterations to get average time value
    :return: dictionary with schema process_count -> avg_time
    """

    result = dict()
    for n_proc in range(1, 24):
        time_values = []
        for i in range(iter):
            output = subprocess.run(
                ["mpirun", "-n", str(n_proc), "--oversubscribe", program_name, "-s", "1000000"],
                capture_output=True,
                encoding="utf-8"
            ).stdout
            print(f"n_proc={n_proc}: {i}/{iter}")
            time_values.append(float(output))

        result[n_proc] = sum(time_values) / len(time_values)

    return result

def plot_graph(output_path: str, X1: tuple, Y1: tuple, X2: tuple, Y2: tuple) -> None:
    """Plot graph to compare execution time of two programs.

    :param output_path: name for graph image file
    :param X1: first program processes counts
    :param Y1: elapsed times for first program
    :param X2: second program processes counts
    :param Y2: elapsed times for second program
    """

    plt.figure(figsize=(10, 6))
    plt.plot(X1, Y1, "b-", linewidth=2, label="Task 1")
    plt.plot(X2, Y2, "r-", linewidth=2, label="Task 2")
    plt.xlabel('Amount of processes', fontsize=12)
    plt.ylabel('Elapsed time', fontsize=12)
    plt.legend(fontsize=10)

    plt.savefig(output_path, dpi=300, bbox_inches="tight", facecolor="white")
    plt.close()
    

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <image_name> <program1> <program2>")
        exit(0)

    X1, Y1 = zip(*measure_time(sys.argv[2]).items())
    X2, Y2 = zip(*measure_time(sys.argv[3]).items())
    plot_graph(sys.argv[1], X1, Y1, X2, Y2)

