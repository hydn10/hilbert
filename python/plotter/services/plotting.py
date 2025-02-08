import numpy as np
import matplotlib.pyplot as plt


def plot_data(file_path: str) -> None:
    data = np.loadtxt(file_path, delimiter=" ")

    x = data[:, 0]

    lnwdth = 0.60

    fig, axs = plt.subplots(2, 1, sharex=True, figsize=(10, 8))
    
    axs[0].plot(x, data[:, 1], label="Sprung mass", color="#EA0988", linewidth=lnwdth)
    axs[0].plot(x, data[:, 2], label="Unsprung mass", color="#1397D8", linewidth=lnwdth)
    axs[0].set_title("Displacements")
    axs[0].set_ylabel("Displacement (m)")
    axs[0].legend(loc="lower right")

    #axs[1].plot(x, data[:, 3], label="Ground", color="orange", linewidth=lnwdth)
    #ax2 = axs[1].twinx()
    #ax2.plot(x, data[:, 4], label="Force", color="blue", linewidth=lnwdth)
    #axs[1].set_title("Ground-Force Values")
    #lines1, labels1 = axs[1].get_legend_handles_labels()
    #lines2, labels2 = ax2.get_legend_handles_labels()
    #axs[1].legend(lines1 + lines2, labels1 + labels2, loc="upper right")

    axs[1].plot(x, np.unwrap(data[:, 6] - data[:, 9]) - 2 * np.pi, label="Ground", color="#591FDD", linewidth=lnwdth)
    axs[1].set_title("Ground-Force Phase Shift")
    axs[1].set_ylim(0, 4)
    axs[1].set_ylabel("Phase Shift (rad)")
    axs[1].legend(loc="upper right")

    plt.xlabel("Time (s)")

    plt.tight_layout()

    plt.show()