#ifndef CWW_LOGGER_H
#define CWW_LOGGER_H
#include <iostream>

class logger
{

public:

    enum class severity
    {
        trace,
        debug,
        information,
        warning,
        error,
        critical
    };

public:

    virtual ~logger() noexcept = default;

public:

    virtual logger const* log(const std::string&, severity) const = 0;

};

#endif //CWW_LOGGER_H
