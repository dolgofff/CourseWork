#ifndef CWW_LOGGER_BUILDER_CONCRETE_H
#define CWW_LOGGER_BUILDER_CONCRETE_H
#include <map>
#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\logger_builder.h"
class logger_builder_concrete final : public logger_builder
{

private:

    std::map<std::string, logger::severity> _construction_info;

public:

    logger_builder* add_stream(std::string const&, logger::severity) override;

    logger* construct() const override;

    logger_builder* config(std::string const& path) override;

};

#endif //CWW_LOGGER_BUILDER_CONCRETE_H
