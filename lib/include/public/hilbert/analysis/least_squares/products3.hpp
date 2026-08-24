#ifndef HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS3_HPP
#define HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS3_HPP


#include <hilbert/analysis/least_squares/products.hpp>


namespace hilbert::analysis
{

template<supported_float Float, std::size_t ResponseCount>
using least_squares_products3 = least_squares_products<Float, 3uz, ResponseCount>;

} // namespace hilbert::analysis

#endif // HILBERT_ANALYSIS_LEAST_SQUARES_PRODUCTS3_HPP
