#ifndef HILBERT_APP_IO_OUTPUT_STREAM_HPP
#define HILBERT_APP_IO_OUTPUT_STREAM_HPP


#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>


namespace hilbert::app::io
{

template<typename WriteOutput>
void
with_output_stream(
    std::optional<std::filesystem::path> const &output_path, std::ostream &default_output, WriteOutput &&write_output);


template<typename WriteOutput>
void
with_output_stream(
    std::optional<std::filesystem::path> const &output_path, std::ostream &default_output, WriteOutput &&write_output)
{
  if (output_path)
  {
    std::ofstream output{*output_path};
    if (!output)
    {
      throw std::runtime_error{"could not open output file: " + output_path->string()};
    }
    std::invoke(std::forward<WriteOutput>(write_output), output);
    output.flush();
    output.close();
    if (!output)
    {
      throw std::runtime_error{"could not complete output file: " + output_path->string()};
    }
  }
  else
  {
    std::invoke(std::forward<WriteOutput>(write_output), default_output);
    default_output.flush();
    if (!default_output)
    {
      throw std::runtime_error{"could not write output"};
    }
  }
}

} // namespace hilbert::app::io

#endif // HILBERT_APP_IO_OUTPUT_STREAM_HPP
