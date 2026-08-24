#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_BASIS3_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_BASIS3_HPP


#include <hilbert/analysis/least_squares/basis.hpp>


namespace hilbert::analysis
{

template<typename First, typename Second, typename Third>
using basis_row3 = basis_row<First, Second, Third>;


template<class FirstFunction, class SecondFunction, class ThirdFunction>
using basis3 = basis<FirstFunction, SecondFunction, ThirdFunction>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_BASIS3_HPP
