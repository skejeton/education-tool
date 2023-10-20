#ifndef HPP_CATEDU_DIAG_DIAG_MONITOR
#define HPP_CATEDU_DIAG_DIAG_MONITOR

#include <string>
#include <vector>

struct DiagMonitorObject
{
    enum class Type
    {
        Value,
        SectionBegin,
        SectionEnd,
    } type;
    std::string name;
    std::string value;
};

using DiagObjectsIterator = std::vector<DiagMonitorObject>&;

struct DiagMonitorPass
{
    std::vector<DiagMonitorObject> objects;

    DiagObjectsIterator iterate();

    static DiagMonitorPass begin();
    void end();

    void begin_section(const char* name);
    void end_section();

    template<class T>
    void push(const char* name, T value)
    {
        objects.push_back(
          { DiagMonitorObject::Type::Value, name, std::to_string(value) });
    }

    template<>
    void push<const char*>(const char* name, const char* value)
    {
        objects.push_back({ DiagMonitorObject::Type::Value, name, value });
    }

    template<>
    void push<std::string>(const char* name, std::string value)
    {
        objects.push_back({ DiagMonitorObject::Type::Value, name, value });
    }
};

#endif // HPP_CATEDU_DIAG_DIAG_MONITOR
