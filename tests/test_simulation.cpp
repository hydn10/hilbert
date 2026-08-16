#include <hilbert/simulation/view.hpp>

#include <ranges>


namespace
{

using test_profile = decltype([](double) { return 0.0; });
using tested_view = hilbert::simulation::simulation_view<double, test_profile>;

static_assert(std::ranges::input_range<tested_view>);
static_assert(std::ranges::sized_range<tested_view>);
static_assert(std::ranges::view<tested_view>);
static_assert(!std::ranges::forward_range<tested_view>);

} // namespace
