#ifndef HILBERTCLI_EXIT_STATUS_HPP
#define HILBERTCLI_EXIT_STATUS_HPP


namespace hilbertcli
{

enum class exit_status : int
{
  success = 0,
  critical_error = 1,
  cli_error = 2,
};


constexpr int
to_exit_code(exit_status status) noexcept
{
  return static_cast<int>(status);
}

} // namespace hilbertcli

#endif // HILBERTCLI_EXIT_STATUS_HPP
