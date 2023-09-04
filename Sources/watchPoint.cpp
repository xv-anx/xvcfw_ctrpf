#include "cheats.hpp"
#include "WatchPointManager.hpp"

namespace CTRPluginFramework
{
    void doWPManager(MenuEntry* entry) {
        static u32 address = 0;
        static u32 byte = 0;
        static bool isWatched = false;
        static bool isFirstTime = true;

        StringVector settings = {"1 byte", "2 byte", "4 byte", "監視アドレス変更", "Read", "Write"};
        Keyboard keyboard("ウォッチポイントの設定\n\n", settings);
        keyboard.IsHexadecimal(true);

        static u32 watchKind = WatchPoint::Write;

        const int kChoice = keyboard.Open();
        switch (kChoice) {
            case 0:
                byte = 1;
                break;

            case 1:
                byte = 2;
                break;

            case 2:
                byte = 4;
                break;

            case 3:
                if (keyboard.Open(address, address) != 0) {
                    isFirstTime = true;
                    return;
                }
                break;

            case 4:
                watchKind = WatchPoint::Read;
                break;

            case 5:
                watchKind = WatchPoint::Write;
                break;

        }

        entry->SetGameFunc([](MenuEntry* entry) {

            if (entry->Hotkeys[0].IsPressed()) {
                if (byte == 0) {
                    MessageBox("データサイズを選択してください。")();
                    return;

                } else if (byte != 0 && isFirstTime) {
                    isFirstTime = false;

                    if (address == 0) {
                        isFirstTime = true;
                        Sleep(Milliseconds(5));
                        MessageBox("アドレスが入力されていません。")();
                        return;

                    } else {
                        WatchPointManager::SetWatchPoint(address, byte, watchKind);
                        isWatched = true;
                        OSD::Notify(Color::Lime << "Enabled Watch Point!");
                    }
                }
            }

            if (entry->Hotkeys[1].IsPressed() && isWatched) {
                isWatched = false;
                isFirstTime = true;
                WatchPointManager::DisableWatchPoint(address);
                OSD::Notify(Color::Red << "Disabled Watch Point!");
            }

            if (entry->Hotkeys[2].IsPressed() && isWatched) {
                Process::Pause();

            } else if (Controller::IsKeysPressed(Key::Start) && Process::IsPaused) Process::Play();
        });
    }
}
