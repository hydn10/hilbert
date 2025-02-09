import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import hilbert, butter, filtfilt


def bandpass_filter(signal, lowcut, highcut, fs, order=3):
    nyquist = 0.5 * fs
    low = lowcut / nyquist
    high = highcut / nyquist
    b, a = butter(order, [low, high], btype='band')
    return filtfilt(b, a, signal)



def plot_data(file_path: str) -> None:
    data = np.loadtxt(file_path, delimiter=" ")

    x = data[:, 0]

    ground = data[:, 3]
    force = data[:, 4]
    ground_phase = data[:, 6]
    ground_freq = data[:, 7]
    force_amp = data[:, 8]
    force_phase = data[:, 9]
    force_freq = data[:, 10]

    sampling_freq = 2000;

    bandforce = bandpass_filter(force, 5, 30, sampling_freq)
    bandground = bandpass_filter(ground, 2, 22, sampling_freq)

    analytic_bandforce = hilbert(bandforce)
    bandforce_phase = np.unwrap(np.angle(analytic_bandforce))

    analytic_bandground = hilbert(bandground)
    bandground_phase = np.angle(analytic_bandground)

    lnwdth = 0.60

    fig, axs = plt.subplots(2, 1, sharex=True, figsize=(10, 8))
    
    #axs[0].plot(x, ground, label="Sprung mass", color="#EA0988", linewidth=lnwdth)
    #axs2 =axs[0].twinx()
    #axs2.plot(x, normforce, label="Unsprung mass", color="#1397D8", linewidth=lnwdth)
    #axs[0].set_title("Displacements")
    #axs[0].set_ylabel("Displacement (m)")
    #axs[0].legend(loc="lower right")

    #axs[1].plot(x, data[:, 3], label="Ground", color="orange", linewidth=lnwdth)
    #ax2 = axs[1].twinx()
    #ax2.plot(x, data[:, 4], label="Force", color="blue", linewidth=lnwdth)
    #axs[1].set_title("Ground-Force Values")
    #lines1, labels1 = axs[1].get_legend_handles_labels()
    #lines2, labels2 = ax2.get_legend_handles_labels()
    #axs[1].legend(lines1 + lines2, labels1 + labels2, loc="upper right")

    axs[0].plot(x, np.unwrap(ground_phase - force_phase) + -2 * np.pi, label="orig", color="red", linewidth=lnwdth)
    #axs[0].plot(x, force, label="phase", color="red", linewidth=lnwdth)
    axs[0].set_title("Ground-Force Phase Shift")
    axs[0].set_ylim(0, 4)
    axs[0].set_ylabel("Phase Shift (rad)")
    axs[0].legend(loc="upper right")

    axs[1].plot(x, np.unwrap(bandground_phase - force_phase) + -4 * np.pi, label="norm", color="blue", linewidth=lnwdth)
    #axs[1].plot(x, np.unwrap(bandground_phase - normforce_phase) + 2 * np.pi, label="adapt", color="red", linewidth=lnwdth)
    axs[1].set_title("Ground-Force Phase Shift")
    axs[1].set_ylim(0, 4)
    axs[1].set_ylabel("Phase Shift (rad)")
    axs[1].legend(loc="upper right")

    plt.xlabel("Time (s)")

    plt.tight_layout()

    plt.show()