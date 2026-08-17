#ifndef HILBERT_APP_CLI_ERROR_HPP
#define HILBERT_APP_CLI_ERROR_HPP


#include <stdexcept>


namespace hilbert::app::cli
{

class error final : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

} // namespace hilbert::app::cli

#endif // HILBERT_APP_CLI_ERROR_HPP
