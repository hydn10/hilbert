#ifndef HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_RESULT_HPP
#define HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_RESULT_HPP


#include <hilbert/analysis/phase/mean_resultant_length.hpp>
#include <hilbert/analysis/phase/principal_phase.hpp>
#include <hilbert/core/supported_float.hpp>


namespace hilbert::analysis
{

template<supported_float Float>
class circular_mean_result
{
  principal_phase<Float> phase_;
  mean_resultant_length<Float> resultant_length_;

public:
  circular_mean_result(principal_phase<Float> phase, mean_resultant_length<Float> resultant_length);

  [[nodiscard]]
  principal_phase<Float>
  phase() const noexcept;

  [[nodiscard]]
  mean_resultant_length<Float>
  resultant_length() const noexcept;
};


template<supported_float Float>
circular_mean_result<Float>::circular_mean_result(
    principal_phase<Float> phase, mean_resultant_length<Float> resultant_length)
    : phase_{phase}
    , resultant_length_{resultant_length}
{
}


template<supported_float Float>
principal_phase<Float>
circular_mean_result<Float>::phase() const noexcept
{
  return phase_;
}


template<supported_float Float>
mean_resultant_length<Float>
circular_mean_result<Float>::resultant_length() const noexcept
{
  return resultant_length_;
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_CIRCULAR_MEAN_RESULT_HPP
