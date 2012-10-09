#ifndef LOGGING_HPP
#define LOGGING_HPP

#include "programoptions.hpp"
#define VERBOSE(level,message)  if (ProgramOptions::getInstance()->verbosity_level >= level) std::cout << message;

#define VERBOSE_LOW  0
#define VERBOSE_MID  1
#define VERBOSE_HIGH 2

#endif //LOGGING_HPP
