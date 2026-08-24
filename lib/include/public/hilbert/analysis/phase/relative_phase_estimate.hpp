#ifndef HILBERT_ANALYSIS_PHASE_RELATIVE_PHASE_ESTIMATE_HPP
#define HILBERT_ANALYSIS_PHASE_RELATIVE_PHASE_ESTIMATE_HPP


#include <hilbert/analysis/phase/mean_resultant_length.hpp>
#include <hilbert/analysis/phase/principal_phase.hpp>
#include <hilbert/core/supported_float.hpp>


namespace hilbert::analysis
{

template<supported_float Float>
class relative_phase_estimate
{
  principal_phase<Float> phase_;
  mean_resultant_length<Float> resultant_length_;

public:
  relative_phase_estimate(principal_phase<Float> phase, mean_resultant_length<Float> resultant_length);

  [[nodiscard]]
  principal_phase<Float>
  phase() const;

  [[nodiscard]]
  mean_resultant_length<Float>
  resultant_length() const;
};


template<supported_float Float>
relative_phase_estimate<Float>::relative_phase_estimate(
    principal_phase<Float> phase, mean_resultant_length<Float> resultant_length)
    : phase_{phase}
    , resultant_length_{resultant_length}
{
}


template<supported_float Float>
principal_phase<Float>
relative_phase_estimate<Float>::phase() const
{
  return phase_;
}


template<supported_float Float>
mean_resultant_length<Float>
relative_phase_estimate<Float>::resultant_length() const
{
  return resultant_length_;
}

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_PHASE_RELATIVE_PHASE_ESTIMATE_HPP
