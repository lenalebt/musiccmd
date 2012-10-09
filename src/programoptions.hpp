#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP

#include <boost/program_options.hpp>

namespace po = boost::program_options;

/**
 * @brief Parses the program options from the command line.
 * 
 * @param argc The argument count
 * @param argv The argument values
 * 
 * @return <code>0</code> if everything went fine, <code>ERROR_CODE</code> to return from
 *      <code>int main()</code> otherwise.
 */
int parseProgramOptions(int argc, char* argv[], po::variables_map& vm);

#endif      //PROGRAMOPTIONS_HPP
