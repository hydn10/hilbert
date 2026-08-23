#ifndef HILBERT_CORE_SUPPORTED_FLOAT_HPP
#define HILBERT_CORE_SUPPORTED_FLOAT_HPP


#include <concepts>


namespace hilbert
{

template<typename Float>
concept supported_float = std::same_as<Float, double>;

} // namespace hilbert

#endif // HILBERT_CORE_SUPPORTED_FLOAT_HPP
