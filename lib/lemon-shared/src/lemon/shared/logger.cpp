#include <lemon/shared/logger.h>

void
lemon::logger::setLogLevel(lemon::logger::LogLevel level)
{
    lemon::logger::detail::Logger::get().setLogLevel(level);
}