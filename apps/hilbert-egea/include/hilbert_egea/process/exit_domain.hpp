#ifndef HILBERT_EGEA_PROCESS_EXIT_DOMAIN_HPP
#define HILBERT_EGEA_PROCESS_EXIT_DOMAIN_HPP


#include <concepts>
#include <type_traits>
#include <variant>


namespace hilbert_egea::process
{

struct successful_exit_outcome
{
};


struct failed_exit_outcome
{
};


template<int ExitCode>
struct basic_success_outcome : successful_exit_outcome
{
  static constexpr int exit_code = ExitCode;
};


template<typename Domain, int ExitCode>
struct basic_failure_outcome : failed_exit_outcome
{
  using domain = Domain;
  static constexpr int exit_code = ExitCode;
};


template<typename... Types>
struct unique_types : std::true_type
{
};


template<typename First, typename... Rest>
struct unique_types<First, Rest...>
    : std::bool_constant<(!std::same_as<First, Rest> && ...) && unique_types<Rest...>::value>
{
};


template<typename Domain, typename... Outcomes>
requires(sizeof...(Outcomes) > 0uz)
struct failure_group
{
  using domain = Domain;
  using failures = std::variant<Outcomes...>;

  static_assert(
      (std::derived_from<Outcomes, failed_exit_outcome> && ...), "a failure group may contain only failure outcomes");
  static_assert(
      (std::same_as<typename Outcomes::domain, Domain> && ...),
      "every outcome in a failure group must belong to that group");
  static_assert(unique_types<Outcomes...>::value, "an outcome may appear only once in a failure group");

  template<typename Success>
  using result = std::variant<Success, Outcomes...>;

  template<int ExitCode>
  static constexpr bool excludes_exit_code = ((Outcomes::exit_code != ExitCode) && ...);

  template<typename OtherGroup>
  static constexpr bool has_disjoint_exit_codes = (OtherGroup::template excludes_exit_code<Outcomes::exit_code> && ...);

  template<typename Outcome>
  static constexpr bool contains = (std::same_as<std::remove_cvref_t<Outcome>, Outcomes> || ...);
};


template<typename Domain, typename... Groups>
struct find_failure_group;


template<typename Domain, typename First, typename... Rest>
struct find_failure_group<Domain, First, Rest...> : find_failure_group<Domain, Rest...>
{
};


template<typename Domain, typename... Outcomes, typename... Rest>
struct find_failure_group<Domain, failure_group<Domain, Outcomes...>, Rest...>
{
  using type = failure_group<Domain, Outcomes...>;
};


template<typename... Groups>
struct groups_have_disjoint_exit_codes : std::true_type
{
};


template<typename First, typename... Rest>
struct groups_have_disjoint_exit_codes<First, Rest...>
    : std::bool_constant<
          (First::template has_disjoint_exit_codes<Rest> && ...) && groups_have_disjoint_exit_codes<Rest...>::value>
{
};


template<typename Success, typename... FailureGroups>
struct exit_domain
{
  static_assert(std::derived_from<Success, successful_exit_outcome>, "an exit domain must declare one success outcome");
  static_assert(
      unique_types<typename FailureGroups::domain...>::value,
      "an exit domain may contain only one failure group for each domain");
  static_assert(
      (FailureGroups::template excludes_exit_code<Success::exit_code> && ...),
      "the success exit code must not be used by a failure outcome");
  static_assert(
      groups_have_disjoint_exit_codes<FailureGroups...>::value,
      "exit codes may be shared within a failure group, but not across failure groups");

  template<typename Outcome>
  static constexpr bool contains =
      std::same_as<std::remove_cvref_t<Outcome>, Success> || (FailureGroups::template contains<Outcome> || ...);

  template<typename Domain>
  using failure_group_for = typename find_failure_group<Domain, FailureGroups...>::type;

  template<typename Domain>
  using result_for = typename failure_group_for<Domain>::template result<Success>;

  template<typename Domain>
  using failures_for = typename failure_group_for<Domain>::failures;

  template<typename Outcome>
  requires(contains<Outcome>)
  static constexpr int
  to_exit_code(Outcome) noexcept
  {
    return std::remove_cvref_t<Outcome>::exit_code;
  }

  template<typename... Outcomes>
  requires(contains<Outcomes> && ...)
  static constexpr int
  to_exit_code(std::variant<Outcomes...> const &result) noexcept
  {
    return std::visit(
        [](auto outcome) noexcept
        {
          return exit_domain::to_exit_code(outcome);
        },
        result);
  }
};

} // namespace hilbert_egea::process

#endif // HILBERT_EGEA_PROCESS_EXIT_DOMAIN_HPP
