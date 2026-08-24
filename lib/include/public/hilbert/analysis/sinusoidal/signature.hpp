#ifndef HILBERT_ANALYSIS_SINUSOIDAL_SIGNATURE_HPP
#define HILBERT_ANALYSIS_SINUSOIDAL_SIGNATURE_HPP


#include <hilbert/math/linear_algebra/signature.hpp>


namespace hilbert::analysis
{

struct cosine_term
{
};

struct sine_term
{
};

struct constant_term
{
};

using sinusoidal_signature = math::signature<cosine_term, sine_term, constant_term>;


template<typename Signature>
concept sinusoidal_coordinate_signature =
    math::signature_type<Signature> && (Signature::size == 3uz) &&
    math::contains_tag_v<Signature, cosine_term> && math::contains_tag_v<Signature, sine_term> &&
    math::contains_tag_v<Signature, constant_term>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_SINUSOIDAL_SIGNATURE_HPP
