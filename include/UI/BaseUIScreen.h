#ifndef BAKERMAKER_BASEUISCREEN_H
#define BAKERMAKER_BASEUISCREEN_H

#include <map>

#include "imgui.h"

namespace bakermaker {
    // List of fonts pre-loaded at setup
    extern ImFont** fontlist;

    // Enum for all sections of program, in the order they should appear
    enum class ProgramStage {
        SERVER_SETUP,
        SERVER_CONNECT,
        SSH_KEYGEN_ADMIN,
        SERVER_INSTALL,
        SYNC_TO_SERVER,
        USER_MANAGE,
        REPO_MANAGE,
        USED_STORAGE,
        CLIENT_EXTRACT,
        DEBUG_EXPORT,

        LIBS_NOT_FOUND
    };

    // Base UI class for when children are constructed, they can automatically be added to lists
    class BaseUIScreen {
    public:
        explicit BaseUIScreen(ProgramStage programStage, std::map<ProgramStage, BaseUIScreen*>* map);
        virtual void render() = 0;
    };

    // The two lists containing the groups (either setup or post-setup)
    extern std::map<ProgramStage, BaseUIScreen*> setupScreens;
    extern std::map<ProgramStage, BaseUIScreen*> configScreens;
}

#endif //BAKERMAKER_BASEUISCREEN_H
