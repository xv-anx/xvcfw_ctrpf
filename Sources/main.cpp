#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>

#include <vector>
#include "cheats.hpp"

namespace CTRPluginFramework
{
    // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
    static void    ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    static MenuEntry* EntryWithHotkey(MenuEntry* entry, const Hotkey& hotkey) {
        if (entry != nullptr) {
            entry->Hotkeys += hotkey;
            entry->SetArg(new std::string(entry->Name()));
            entry->Name() += " " + hotkey.ToString();
            entry->Hotkeys.OnHotkeyChangeCallback([](MenuEntry* entry, int index) {
                auto* name = reinterpret_cast<std::string*>(entry->GetArg());
                entry->Name() = *name + " " + entry->Hotkeys[0].ToString();
            });
        }
        return entry;
    }

    static MenuEntry* EntryWithHotkey(MenuEntry* entry,
                                    const std::vector<Hotkey>& hotkeys) {
        for (const Hotkey& kHotkey : hotkeys) {
            entry->Hotkeys += kHotkey;
        }
        return entry;
    }

    static MenuEntry* EnableEntry(MenuEntry* entry) {
        if (entry != nullptr) {
            entry->SetArg(new std::string(entry->Name()));
            entry->Enable();
        }
        return entry;
    }
    
    // This function is called before main and before the game starts
    // Useful to do code edits safely
    void    PatchProcess(FwkSettings &settings)
    {
        ToggleTouchscreenForceOn();
    }

    // This function is called when the process exits
    // Useful to save settings, undo patchs or clean up things
    void    OnProcessExit(void)
    {
        ToggleTouchscreenForceOn();
    }

    void    InitMenu(PluginMenu &menu)
    {
        menu += new MenuEntry(Color::Yellow << "パッチコード作成自動化", nullptr, autoPatchCode, "開始アドレスと終了アドレスを設定してください。\n開始アドレスと終了アドレスが同じ場合は、アドレスの初期化をしてください。\n\n次に [ファイルに書き込み] を押下してください。\n\n/lumna/plugins/" + getFilePath() + " にコードが作成されます。\n\n" + (Color::Red << "※注意\n") + "CTRPF上でコードが作成されたファイルを開くと、\n新しくコードが作成できません。その場合はゲームを再起動してからコードを作成してください。\n\n" + (Color::White << "Enjoy coding!"));
        menu += EntryWithHotkey(new MenuEntry(Color::Orange << "ウォッチポイント（逆アセ）", nullptr, doWPManager), {Hotkey(Key::R | Y, "監視開始"), Hotkey(Key::R | X, "監視終了"), Hotkey(Key::R | A, "一時停止")});
        menu += EntryWithHotkey(new MenuEntry(Color::Turquoise << "レジスタ確認", regConf), {Hotkey(Key::L | Y, "アドレス変更"), Hotkey(Key::L | X, "エントリー無効")});
        menu += new MenuEntry(Color::Lime << "16進数電卓", nullptr, calcHex);
    }

    int     main(void)
    {
        PluginMenu *menu = new PluginMenu(Color::Red << "[Beta] ASMerCTRPF", 0, 7, 4,
                                            "作成者: xv\nTwitter: @xv_anx\nYouTube: xvcfw\nDiscord: xvcfw");

        // Synnchronize the menu with frame event
        menu->SynchronizeWithFrame(true);

        // Init our menu entries & folders
        InitMenu(*menu);

        // Launch menu and mainloop
        menu->Run();

        delete menu;

        // Exit plugin
        return (0);
    }
}
