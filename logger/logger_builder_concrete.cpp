#include "C:\Users\Sergey\source\repos\MyCourseWork\MyCourseWork\logger_concrete.h"
#include <map>
#include <fstream>

logger_builder* logger_builder_concrete::add_stream(std::string const& path, logger::severity severity)
{
    _construction_info[path] = severity;

    return this;
}

logger* logger_builder_concrete::construct() const
{
    return new logger_concrete(_construction_info);
}

logger_builder* logger_builder_concrete::config(std::string const& path) {
    std::string file_path, severity;
    std::map<std::string, logger::severity> severity_types = { {"trace",logger::severity::trace},
                                                               {"debug",logger::severity::debug},
                                                               {"information",logger::severity::information},
                                                               {"warning",logger::severity::warning},
                                                               {"error",logger::severity::error},
                                                               {"critical",logger::severity::critical} };
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) throw std::logic_error("File not open!");
    while (file >> file_path >> severity) {
        add_stream(file_path, severity_types[severity]);
    }
    file.close();
    return this;
}