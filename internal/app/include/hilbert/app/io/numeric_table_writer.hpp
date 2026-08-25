#ifndef HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP
#define HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP


#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <limits>
#include <ostream>
#include <string_view>
#include <utility>


namespace hilbert::app::io
{

template<typename Value>
concept stream_insertable = requires(std::ostream &output, Value const &value) { output << value; };


template<typename WriteRows, typename Rows>
concept table_writer_for = std::invocable<WriteRows, Rows &>;


template<std::floating_point Float>
class numeric_table_writer
{
  template<size_t ColumnCount>
  class table_rows;

  std::reference_wrapper<std::ostream> output_;
  bool has_table_ = false;

public:
  explicit numeric_table_writer(std::ostream &output) noexcept;

  numeric_table_writer(numeric_table_writer const &) = delete;
  numeric_table_writer &
  operator=(numeric_table_writer const &) = delete;
  numeric_table_writer(numeric_table_writer &&) = delete;
  numeric_table_writer &
  operator=(numeric_table_writer &&) = delete;
  ~numeric_table_writer() = default;

  template<size_t ColumnCount, table_writer_for<table_rows<ColumnCount>> WriteRows>
  void
  table(std::string_view name, std::array<std::string_view, ColumnCount> const &columns, WriteRows &&write_rows);
};


template<std::floating_point Float>
template<size_t ColumnCount>
class numeric_table_writer<Float>::table_rows
{
  std::reference_wrapper<std::ostream> output_;

public:
  explicit table_rows(std::ostream &output) noexcept;

  table_rows(table_rows const &) = delete;
  table_rows &
  operator=(table_rows const &) = delete;
  table_rows(table_rows &&) = delete;
  table_rows &
  operator=(table_rows &&) = delete;
  ~table_rows() = default;

  template<stream_insertable... Values>
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
template<stream_insertable... Values>
requires(sizeof...(Values) == ColumnCount)
void
numeric_table_writer<Float>::table_rows<ColumnCount>::write(Values const &...values)
{
  size_t column_index = 0uz;
  ((output_.get() << (column_index++ == 0uz ? "" : ",") << values), ...);
  output_.get() << '\n';
}


template<std::floating_point Float>
numeric_table_writer<Float>::numeric_table_writer(std::ostream &output) noexcept
    : output_{output}
{
  output_.get() << std::setprecision(std::numeric_limits<Float>::max_digits10);
}


template<std::floating_point Float>
template<
    size_t ColumnCount,
    table_writer_for<typename numeric_table_writer<Float>::template table_rows<ColumnCount>> WriteRows>
void
numeric_table_writer<Float>::table(
    std::string_view name, std::array<std::string_view, ColumnCount> const &columns, WriteRows &&write_rows)
{
  if (has_table_)
  {
    output_.get() << '\n';
  }

  output_.get() << "# table: " << name << '\n';
  bool first_column = true;
  for (auto const column : columns)
  {
    if (!first_column)
    {
      output_.get() << ',';
    }
    output_.get() << column;
    first_column = false;
  }
  output_.get() << '\n';

  table_rows<ColumnCount> rows{output_};
  std::forward<WriteRows>(write_rows)(rows);
  has_table_ = true;
}

} // namespace hilbert::app::io

#endif // HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP
