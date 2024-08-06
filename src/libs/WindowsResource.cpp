#include "WindowsResource.h"

namespace bakermaker {
    WindowsResource::WindowsResource(const ST::string& path, const ST::string& type) {
        info = FindResourceA(nullptr, path.c_str(), type.c_str());
        resource = LoadResource(nullptr, info);
        size = SizeofResource(nullptr, info);
        bdata = static_cast<LPCSTR>(LockResource(resource));
    }

    WindowsResource::~WindowsResource() {
        UnlockResource(resource);
        FreeResource(resource);
    }

    LPCSTR WindowsResource::getData() const {
        return bdata;
    }

    DWORD WindowsResource::getSize() const {
        return size;
    }




}