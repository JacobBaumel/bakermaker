#ifndef RESOURCE_H
#define RESOURCE_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "string_theory/string"

namespace bakermaker {
    class WindowsResource {
        LPCSTR bdata;
        HGLOBAL resource;
        HRSRC info;
        DWORD size;

    public:
        explicit WindowsResource(const ST::string& path, const ST::string& type);
        ~WindowsResource();
        LPCSTR getData() const;
        DWORD getSize() const;
    };
}

#endif //RESOURCE_H
