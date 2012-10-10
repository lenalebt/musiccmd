#ifndef LOGGING_HPP
#define LOGGING_HPP

#include "programoptions.hpp"
#define VERBOSE(level,message)          if (ProgramOptions::getInstance()->verbosity_level >= level) std::cout << message;
#define VERBOSE_DB(level,message)       if (ProgramOptions::getInstance()->db_verbosity_level >= level) std::cout << message;

#define VERBOSE_LOW  0
#define VERBOSE_MID  1
#define VERBOSE_HIGH 2

#if 0
class noop_streambuf : public std::streambuf
{
public:
    int overflow(int c) {return c;}
};

noop_streambuf null_buf;
std::ostream noop_os(null_buf);

class log_ostream
{
private:
    unsigned int level;
    unsigned int& global_level;
protected:
    
public:
    log_ostream(unsigned int level, unsigned int& global_level = ProgramOptions::getInstance()->verbosity_level) :
        level(level), global_level(global_level) {}
    friend std::ostream& operator<<(std::ostream& os, log_ostream& log);
};

std::ostream& operator<<(std::ostream& os, log_ostream& log);
#endif

#endif //LOGGING_HPP
