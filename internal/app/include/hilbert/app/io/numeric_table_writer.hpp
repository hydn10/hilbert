#ifndef HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP
#define HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP


#include <array>
#include <concepts>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <ostream>
#include <string_view>
#include <utility>


namespace hilbert::app::io
{

template<std::floating_point Float>
class numeric_table_writer
{
  template<size_t ColumnCount>
  class table_rows;

  std::ostream &output_;
  bool has_table_ = false;

public:
  explicit numeric_table_writer(std::ostream &output) noexcept;

  template<size_t ColumnCount, typename WriteRows>
  void
  table(std::string_view name, std::array<std::string_view, ColumnCount> const &columns, WriteRows &&write_rows);
};


template<std::floating_point Float>
template<size_t ColumnCount>
class numeric_table_writer<Float>::table_rows
{
  std::ostream &output_;

public:
  explicit table_rows(std::ostream &output) noexcept;

  template<typename... Values>
  requires(sizeof...(Values) == ColumnCount)
  void
  write(Values const &...values);
};


template<std::floating_point Float>
template<size_t ColumnCount>
numeric_table_writer<Float>::table_rows<ColumnCount>::table_rows(std::ostream &output) noexcept
    : output_{output}
{
}


template<std::floating_point Float>
template<size_t ColumnCount>
template<typename... Values>
requires(sizeof...(Values) == ColumnCount)
void
numeric_table_writer<Float>::table_rows<ColumnCount>::write(Values const &...values)
{
  size_t column_index = 0uz;
  ((output_ << (column_index++ == 0uz ? "" : ",") << values), ...);
  output_ << '\n';
}


template<std::floating_point Float>
numeric_table_writer<Float>::numeric_table_writer(std::ostream &output) noexcept
    : output_{output}
{
  output_ << std::setprecision(std::numeric_limits<Float>::max_digits10);
}


template<std::floating_point Float>
template<size_t ColumnCount, typename WriteRows>
void
numeric_table_writer<Float>::table(
    std::string_view name, std::array<std::string_view, ColumnCount> const &columns, WriteRows &&write_rows)
{
  if (has_table_)
  {
    output_ << '\n';
  }

  output_ << "# table: " << name << '\n';
  for (size_t column_index = 0uz; column_index < ColumnCount; ++column_index)
  {
    if (column_index != 0uz)
    {
      output_ << ',';
    }
    output_ << columns[column_index];
  }
  output_ << '\n';

  table_rows<ColumnCount> rows{output_};
  std::forward<WriteRows>(write_rows)(rows);
  has_table_ = true;
}

} // namespace hilbert::app::io

#endif // HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP
