from dataclasses import dataclass

import numpy as np

from ..egea_data import EgeaData


@dataclass(frozen=True)
class EgeaMeasurementPlotData:
    simulation: EgeaData
    measurement_start_s: float
    measurement_end_s: float
    raw_samples: np.ndarray
    refined_samples: np.ndarray
    raw_time_s: np.ndarray
    refined_time_s: np.ndarray
    phase_shift_rad: np.ndarray
    platform_frequency_hz: np.ndarray


def positive_continuous_egea_relative_phase(
    first_phase: np.ndarray,
    second_phase: np.ndarray,
) -> np.ndarray:
    wrapped_relative_phase = np.angle(np.exp(1j * (first_phase - second_phase)))
    relative_phase = np.unwrap(wrapped_relative_phase)

    full_turn = 2 * np.pi
    full_turns_to_add = max(
        0,
        int(np.floor(-np.min(relative_phase) / full_turn)) + 1,
    )
    return relative_phase + full_turns_to_add * full_turn


def _interval_mask(time: np.ndarray, start_time: float, end_time: float) -> np.ndarray:
    return (time >= start_time) & (time < end_time)


def prepare_egea_measurement_plot_data(simulation: EgeaData) -> EgeaMeasurementPlotData:
    measurement_start = simulation.intervals.measurement_start_s
    measurement_end = simulation.intervals.measurement_end_s

    raw_samples = _interval_mask(simulation.raw["time_s"], measurement_start, measurement_end)
    refined_samples = _interval_mask(
        simulation.refined["time_s"], measurement_start, measurement_end
    )
    phase_shift = positive_continuous_egea_relative_phase(
        simulation.refined["platform_phase_rad"],
        simulation.refined["tire_force_phase_rad"],
    )[refined_samples]

    return EgeaMeasurementPlotData(
        simulation=simulation,
        measurement_start_s=measurement_start,
        measurement_end_s=measurement_end,
        raw_samples=raw_samples,
        refined_samples=refined_samples,
        raw_time_s=simulation.raw["time_s"][raw_samples],
        refined_time_s=simulation.refined["time_s"][refined_samples],
        phase_shift_rad=phase_shift,
        platform_frequency_hz=simulation.refined["platform_frequency_hz"][refined_samples],
    )
