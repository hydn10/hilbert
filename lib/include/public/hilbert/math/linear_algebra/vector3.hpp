#ifndef HILBERT_MATH_LINEAR_ALGEBRA_VECTOR3_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_VECTOR3_HPP


#include <hilbert/math/linear_algebra/vector.hpp>


namespace hilbert::math
{

template<supported_float Float, coordinate_signature Signature>
requires(Signature::size == 3uz)
using vector3 = vector<Float, Signature>;

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_VECTOR3_HPP
