#ifndef HILBERT_ANALYSIS_PHASE_HPP
#define HILBERT_ANALYSIS_PHASE_HPP


#include <hilbert/hilbert.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <numbers>
#include <span>
#include <stdexcept>
#include <vector>


namespace hilbert::analysis
{

template<supported_float Float>
struct time_window
{
  Float begin;
  Float end;
};


template<supported_float Float>
struct sinusoidal_fit
{
  Float cosine_coefficient;
  Float sine_coefficient;
  Float dc_offset;
  Float amplitude;
};


template<supported_float Float>
struct sinusoidal_phase_result
{
  sinusoidal_fit<Float> ground;
  sinusoidal_fit<Float> force;
  Float phase_rad;
};


template<supported_float Float>
struct hilbert_phase_result
{
  Float phase_rad;
  Float mean_resultant_length;
};


namespace detail
{

template<supported_float Float>
struct measurement_range
{
  size_t begin;
  size_t end;
};


template<supported_float Float>
void
validate_window(time_window<Float> window)
{
  if (!std::isfinite(window.begin) || !std::isfinite(window.end) || window.begin >= window.end)
  {
    throw std::invalid_argument{"analysis window must have finite, increasing bounds"};
  }
}


template<supported_float Float>
void
validate_channels(std::span<Float const> time, std::span<Float const> ground, std::span<Float const> force)
{
  if (time.size() != ground.size() || time.size() != force.size())
  {
    throw std::invalid_argument{"analysis channels must have equal lengths"};
  }
  if (time.size() < 2uz)
  {
    throw std::invalid_argument{"phase analysis requires at least two samples"};
  }

  for (size_t index = 0uz; index < time.size(); ++index)
  {
    if (!std::isfinite(time[index]) || (index > 0uz && time[index] <= time[index - 1uz]))
    {
      throw std::invalid_argument{"analysis time samples must be finite and strictly increasing"};
    }
  }
}


template<supported_float Float>
measurement_range<Float>
select_measurement_range(std::span<Float const> time, time_window<Float> window)
{
  validate_window(window);

  size_t first = 0uz;
  while (first < time.size() && time[first] < window.begin)
  {
    ++first;
  }

  size_t last = first;
  while (last < time.size() && time[last] < window.end)
  {
    ++last;
  }

  if (last - first < 2uz)
  {
    throw std::invalid_argument{"analysis window contains too few samples"};
  }

  return {first, last};
}


template<supported_float Float>
void
validate_values(std::span<Float const> values, measurement_range<Float> range)
{
  for (auto index = range.begin; index < range.end; ++index)
  {
    if (!std::isfinite(values[index]))
    {
      throw std::invalid_argument{"analysis values must be finite"};
    }
  }
}


template<supported_float Float>
struct gram_matrix
{
  Float cosine_cosine{};
  Float cosine_sine{};
  Float cosine_constant{};
  Float sine_sine{};
  Float sine_constant{};
  Float constant_constant{};
};


template<supported_float Float>
struct cholesky_3x3
{
  Float l00;
  Float l10;
  Float l11;
  Float l20;
  Float l21;
  Float l22;

  static cholesky_3x3
  factor(gram_matrix<Float> const &gram)
  {
    auto const require_positive_finite = [](Float value)
    {
      if (!std::isfinite(value) || value <= 0)
      {
        throw std::invalid_argument{"sinusoidal fit basis is degenerate"};
      }
      return value;
    };

    auto const l00 = std::sqrt(require_positive_finite(gram.cosine_cosine));
    auto const l10 = gram.cosine_sine / l00;
    auto const l20 = gram.cosine_constant / l00;
    auto const l11_squared = gram.sine_sine - l10 * l10;
    auto const l11 = std::sqrt(require_positive_finite(l11_squared));
    auto const l21 = (gram.sine_constant - l20 * l10) / l11;
    auto const l22_squared = gram.constant_constant - l20 * l20 - l21 * l21;
    auto const l22 = std::sqrt(require_positive_finite(l22_squared));

    if (!std::isfinite(l10) || !std::isfinite(l20) || !std::isfinite(l21))
    {
      throw std::invalid_argument{"sinusoidal fit basis is degenerate"};
    }

    return {l00, l10, l11, l20, l21, l22};
  }

  [[nodiscard]]
  std::array<Float, 3>
  solve(std::array<Float, 3> const &right_hand_side) const
  {
    auto const y0 = right_hand_side[0] / l00;
    auto const y1 = (right_hand_side[1] - l10 * y0) / l11;
    auto const y2 = (right_hand_side[2] - l20 * y0 - l21 * y1) / l22;

    auto const x2 = y2 / l22;
    auto const x1 = (y1 - l21 * x2) / l11;
    auto const x0 = (y0 - l10 * x1 - l20 * x2) / l00;

    std::array<Float, 3> result{x0, x1, x2};
    if (!std::all_of(
            result.begin(),
            result.end(),
            [](Float value)
            {
              return std::isfinite(value);
            }))
    {
      throw std::invalid_argument{"sinusoidal fit produced non-finite coefficients"};
    }
    return result;
  }
};


template<supported_float Float>
sinusoidal_fit<Float>
make_sinusoidal_fit(std::array<Float, 3> const &coefficients)
{
  auto const amplitude = std::hypot(coefficients[0], coefficients[1]);
  if (!std::isfinite(amplitude))
  {
    throw std::invalid_argument{"sinusoidal fit produced a non-finite amplitude"};
  }

  return {
      .cosine_coefficient = coefficients[0],
      .sine_coefficient = coefficients[1],
      .dc_offset = coefficients[2],
      .amplitude = amplitude,
  };
}


template<supported_float Float>
std::vector<Float>
remove_dc_component(std::span<Float const> signal)
{
  Float sum = 0;
  for (auto const value : signal)
  {
    if (!std::isfinite(value))
    {
      throw std::invalid_argument{"Hilbert input values must be finite"};
    }
    sum += value;
  }
  if (!std::isfinite(sum))
  {
    throw std::invalid_argument{"Hilbert input mean is non-finite"};
  }

  auto const mean = sum / static_cast<Float>(signal.size());
  std::vector<Float> centered(signal.size());
  for (size_t index = 0uz; index < signal.size(); ++index)
  {
    centered[index] = signal[index] - mean;
    if (!std::isfinite(centered[index]))
    {
      throw std::invalid_argument{"Hilbert input cannot be centered"};
    }
  }
  return centered;
}

} // namespace detail


// Fit both signals with one shared cosine/sine basis and one Cholesky factorization.
template<supported_float Float>
sinusoidal_phase_result<Float>
fit_sinusoidal_phase(
    std::span<Float const> time,
    std::span<Float const> ground,
    std::span<Float const> force,
    Float frequency_hz,
    time_window<Float> window)
{
  if (!std::isfinite(frequency_hz) || frequency_hz <= 0)
  {
    throw std::invalid_argument{"sinusoidal fit frequency must be finite and positive"};
  }

  detail::validate_channels(time, ground, force);
  auto const range = detail::select_measurement_range(time, window);
  detail::validate_values(ground, range);
  detail::validate_values(force, range);

  detail::gram_matrix<Float> gram;
  std::array<Float, 3> ground_right_hand_side{};
  std::array<Float, 3> force_right_hand_side{};
  Float constexpr tau = static_cast<Float>(2) * std::numbers::pi_v<Float>;

  // The basis values are calculated once per sample and feed both right-hand sides.
  for (auto index = range.begin; index < range.end; ++index)
  {
    auto const angle = tau * frequency_hz * time[index];
    auto const cosine = std::cos(angle);
    auto const sine = std::sin(angle);
    auto const ground_value = ground[index];
    auto const force_value = force[index];

    gram.cosine_cosine += cosine * cosine;
    gram.cosine_sine += cosine * sine;
    gram.cosine_constant += cosine;
    gram.sine_sine += sine * sine;
    gram.sine_constant += sine;
    gram.constant_constant += static_cast<Float>(1);

    ground_right_hand_side[0] += cosine * ground_value;
    ground_right_hand_side[1] += sine * ground_value;
    ground_right_hand_side[2] += ground_value;
    force_right_hand_side[0] += cosine * force_value;
    force_right_hand_side[1] += sine * force_value;
    force_right_hand_side[2] += force_value;
  }

  auto const factor = detail::cholesky_3x3<Float>::factor(gram);
  auto const ground_fit = detail::make_sinusoidal_fit(factor.solve(ground_right_hand_side));
  auto const force_fit = detail::make_sinusoidal_fit(factor.solve(force_right_hand_side));

  std::complex<Float> const ground_phasor{ground_fit.cosine_coefficient, -ground_fit.sine_coefficient};
  std::complex<Float> const force_phasor{force_fit.cosine_coefficient, -force_fit.sine_coefficient};
  auto const relative_phasor = ground_phasor * std::conj(force_phasor);
  if (std::abs(relative_phasor) == 0)
  {
    throw std::invalid_argument{"sinusoidal fit has zero signal amplitude"};
  }

  return {
      .ground = ground_fit,
      .force = force_fit,
      .phase_rad = std::arg(relative_phasor),
  };
}


template<supported_float Float>
hilbert_phase_result<Float>
estimate_hilbert_phase(
    std::span<Float const> time, std::span<Float const> ground, std::span<Float const> force, time_window<Float> window)
{
  detail::validate_channels(time, ground, force);
  auto const centered_ground = detail::remove_dc_component(ground);
  auto const centered_force = detail::remove_dc_component(force);

  // Both transforms deliberately cover the complete simulation record. The window is
  // applied only after the FFT-based analytic signals have been constructed.
  auto const analytic_ground = hilbert_transform<Float>(std::span<Float const>{centered_ground});
  auto const analytic_force = hilbert_transform<Float>(std::span<Float const>{centered_force});
  auto const range = detail::select_measurement_range(time, window);

  std::complex<Float> phase_sum{};
  auto const sample_count = range.end - range.begin;
  for (auto index = range.begin; index < range.end; ++index)
  {
    auto const relative_signal = analytic_ground[index] * std::conj(analytic_force[index]);
    auto const magnitude = std::abs(relative_signal);
    if (!std::isfinite(magnitude) || magnitude == 0)
    {
      throw std::invalid_argument{"Hilbert phase is undefined for a zero signal"};
    }
    phase_sum += relative_signal / magnitude;
  }

  auto const sum_magnitude = std::abs(phase_sum);
  if (!std::isfinite(sum_magnitude) || sum_magnitude == 0)
  {
    throw std::invalid_argument{"Hilbert phase circular mean is undefined"};
  }

  auto const resultant_length = std::min(static_cast<Float>(1), sum_magnitude / static_cast<Float>(sample_count));
  return {
      .phase_rad = std::arg(phase_sum),
      .mean_resultant_length = resultant_length,
  };
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_HPP
