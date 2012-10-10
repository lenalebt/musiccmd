#include "logging.hpp"

#if 0
std::ostream noop_os = std::ostream();

std::ostream& operator<<(std::ostream& os, log_ostream& log)
{
    if (log.level <= log.global_level)
        return os;
    else
        return noop_os;
}
#endif
