#include "diag_monitor.hpp"

DiagObjectsIterator DiagMonitorPass::iterate()
{
    return objects;
}

DiagMonitorPass DiagMonitorPass::begin()
{
    return DiagMonitorPass{};
}

void DiagMonitorPass::end()
{
}

void DiagMonitorPass::begin_section(const char *name)
{
    objects.push_back(
        DiagMonitorObject{DiagMonitorObject::Type::SectionBegin, name, ""});
}

void DiagMonitorPass::end_section()
{
    objects.push_back(
        DiagMonitorObject{DiagMonitorObject::Type::SectionEnd, "", ""});
}
