#ifndef HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX3_HPP
#define HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX3_HPP


#include <hilbert/math/linear_algebra/symmetric_matrix.hpp>


namespace hilbert::math
{

template<supported_float Float, coordinate_signature Signature>
requires(Signature::size == 3uz)
using symmetric_matrix3 = symmetric_matrix<Float, dual_signature_t<Signature>, Signature>;

} // namespace hilbert::math

#endif // HILBERT_MATH_LINEAR_ALGEBRA_SYMMETRIC_MATRIX3_HPP
