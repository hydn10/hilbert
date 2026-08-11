#ifndef HILBERTCLI_OUTPUT_HPP
#define HILBERTCLI_OUTPUT_HPP


#include <iosfwd>


namespace hilbertcli
{

struct simulation_result;


void
write_simulation_data(std::ostream &output, simulation_result const &result);

} // namespace hilbertcli

#endif // HILBERTCLI_OUTPUT_HPP
