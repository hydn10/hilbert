#ifndef HILBERT_DETAIL_ATTRIBUTES_HPP
#define HILBERT_DETAIL_ATTRIBUTES_HPP


#ifdef _MSC_VER
#define HILBERT_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define HILBERT_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#endif // HILBERT_DETAIL_ATTRIBUTES_HPP
