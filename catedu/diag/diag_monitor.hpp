#pragma once
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

using DiagObjectsIterator = std::vector<DiagMonitorObject> &;

struct DiagMonitorPass
{
    std::vector<DiagMonitorObject> objects;

    DiagObjectsIterator iterate();

    static DiagMonitorPass begin();
    void end();

    void begin_section(const char *name);
    void end_section();

    template <class T> void push(const char *name, T value)
    {
        objects.push_back(
            {DiagMonitorObject::Type::Value, name, std::to_string(value)});
    }

    void push(const char *name, const char *value)
    {
        objects.push_back({DiagMonitorObject::Type::Value, name, value});
    }

    void push(const char *name, std::string value)
    {
        objects.push_back({DiagMonitorObject::Type::Value, name, value});
    }
};
