#ifndef CWW_LOGGER_BUILDER_H
#define CWW_LOGGER_BUILDER_H
#include <iostream>
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\logger.h"

class logger_builder
{

public:

    virtual logger_builder* add_stream(std::string const&, logger::severity) = 0;

    virtual logger* construct() const = 0;

    virtual logger_builder* config(std::string const& path) = 0;

public:

    virtual ~logger_builder() noexcept = default;

};

#endif //CWW_LOGGER_BUILDER_H
