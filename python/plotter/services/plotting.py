import numpy as np
import matplotlib.pyplot as plt


def plot_data(file_path: str) -> None:

    data = np.loadtxt(file_path, delimiter=' ')

    # Plot the selected columns
    plt.figure(figsize=(10, 5))
    plt.plot(data[:, 0], data[:, 1], label="Sprung mass", linewidth=0.75)
    plt.plot(data[:, 0], data[:, 2], label="Unsprung mass", linewidth=0.75)

    plt.xlabel("Time (s)")
    plt.ylabel("Displacement (m)")
    plt.legend()
    plt.grid()
    plt.show()