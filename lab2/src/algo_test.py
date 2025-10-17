import sys
import subprocess
import random

def generate_array(size: int):
    """Generate random array.

    :param size: size of array to generate
    :return: list of random values with specified size
    """
    
    return [random.randrange(-100, 100) for _ in range(size)]

def test(prog_name: str):
    for n in range(2, 50):
        for s in range(2, 1000, random.randint(100, 150)):
            array = generate_array(s)
            result = subprocess.run(
                ["mpirun", "-n", str(n), "--oversubscribe", prog_name, "-s", str(s), "-i"],
                input=' '.join([str(x) for x in array]),
                capture_output=True,
                encoding="utf-8"
            )

            passed = True
            if 2 * (n - 1) > s:
                passed = False
            elif result.returncode != 0:
                passed = False
                print(result.stderr)
            elif int(result.stdout) != max(array):
                passed = False
            
            print(f"n={n}, s={s}: {"PASSED" if passed else "FAILED"}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <prog_name>")
        exit(0)

    test(sys.argv[1])
