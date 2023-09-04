#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"

namespace CTRPluginFramework
{
    using StringVector = std::vector<std::string>;
    using OnPauseResumeCallback = void(*)(bool goingToPause);
    std::string getFilePath();

    void autoPatchCode(MenuEntry *entry);
    void doWPManager(MenuEntry* entry);
    void regConf(MenuEntry* entry);
    void calcHex(MenuEntry* entry);
}
#endif
