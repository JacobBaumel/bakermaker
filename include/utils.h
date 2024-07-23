#ifndef BAKERMAKER_UTILS_H
#define BAKERMAKER_UTILS_H

#define USERLENGTH 64

#include "STStringJson.h"
#include "nlohmann/json.hpp"

namespace bakermaker {
    extern nlohmann::json config;

    // Sets the error modal string
    void startErrorModal(const char* error);

    // Actually displays the error modal on top of all other UI
    void displayErrorModal();

    // Simplified spinner function
    void spinner();
}

#endif //BAKERMAKER_UTILS_H
