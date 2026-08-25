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


namespace detail
{

template<size_t ColumnCount>
class numeric_table_writer_rows;

} // namespace detail


template<std::floating_point Float>
class numeric_table_writer
{
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

  template<size_t ColumnCount, table_writer_for<detail::numeric_table_writer_rows<ColumnCount>> WriteRows>
  void
  table(std::string_view name, std::array<std::string_view, ColumnCount> const &columns, WriteRows &&write_rows);
};


namespace detail
{

template<size_t ColumnCount>
class numeric_table_writer_rows
{
  std::reference_wrapper<std::ostream> output_;

public:
  explicit numeric_table_writer_rows(std::ostream &output) noexcept;

  numeric_table_writer_rows(numeric_table_writer_rows const &) = delete;
  numeric_table_writer_rows &
  operator=(numeric_table_writer_rows const &) = delete;
  numeric_table_writer_rows(numeric_table_writer_rows &&) = delete;
  numeric_table_writer_rows &
  operator=(numeric_table_writer_rows &&) = delete;
  ~numeric_table_writer_rows() = default;

  template<stream_insertable... Values>
  requires(sizeof...(Values) == ColumnCount)
  void
  write(Values const &...values);
};

} // namespace detail


template<size_t ColumnCount>
detail::numeric_table_writer_rows<ColumnCount>::numeric_table_writer_rows(std::ostream &output) noexcept
    : output_{output}
{
}


template<size_t ColumnCount>
template<stream_insertable... Values>
requires(sizeof...(Values) == ColumnCount)
void
detail::numeric_table_writer_rows<ColumnCount>::write(Values const &...values)
{
  size_t column_index = 0uz;
  ((output_.get() << (column_index++ == 0uz ? "" : ",") << values), ...);
  output_.get() << '\n';
}


template<std::floating_point Float>
template<size_t ColumnCount, table_writer_for<detail::numeric_table_writer_rows<ColumnCount>> WriteRows>
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

  detail::numeric_table_writer_rows<ColumnCount> rows{output_};
  std::forward<WriteRows>(write_rows)(rows);
  has_table_ = true;
}


template<std::floating_point Float>
numeric_table_writer<Float>::numeric_table_writer(std::ostream &output) noexcept
    : output_{output}
{
  output_.get() << std::setprecision(std::numeric_limits<Float>::max_digits10);
}


} // namespace hilbert::app::io

#endif // HILBERT_APP_IO_NUMERIC_TABLE_WRITER_HPP
