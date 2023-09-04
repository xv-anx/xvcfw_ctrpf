#include "cheats.hpp"

namespace CTRPluginFramework {

    void codeASM(u32 address) {
        u32 const data[] = {
            // Bamboo ASM Code.
            0xE92D3FFF, 0xE10F0000, 0xE52D0004, 0xE28D2004,
            0xE28F3038, 0xE28D403C, 0xE0C200D8, 0xE0C300F8,
            0xE1520004, 0xBAFFFFFB, 0xE49D0004, 0xE129F000,
            0xE59E3008, 0xE59E4010, 0xE58F304C, 0xE58F4044,
            0xE58F0048, 0xE8BD001F, 0xE28DD024, 0xE12FFF1E
        };
        Process::CopyMemory(reinterpret_cast<void*>(address), data, sizeof(data));
    }

    static u32 baseAddress = 0x100000;

    void regConf(MenuEntry* entry) {
        u32 targetAddress = 0x1E81100;
        static u32 initValue = 0;
        static u32 regAddress;
        const Screen &topScr = OSD::GetTopScreen();
        Hook* hook = new Hook();

        // フラグ設定
        static bool isFirstTime = true;
        static bool isCheck = false;
        static bool isHotkey = false;
        static u32  changeAddress = baseAddress;
        static bool breakPoint = true;

        Keyboard const kb("アドレスを入力：レジスタ確認");
        if (entry->Hotkeys[0].IsPressed()) {
            // 最初にボタンを押した処理
            if (isFirstTime) {
                isFirstTime = false;
                if (kb.Open(changeAddress, changeAddress) != 0) {
                    isFirstTime = true;
                    return;
                }

                // キーボードで0x100000以上ならフック開始
                if (changeAddress >= 0x100000) {
                    if((changeAddress & 3) != 0) {
                        Sleep(Milliseconds(5));
                        isFirstTime = true;
                        isHotkey = true;
                        MessageBox("アドレスが無効です。4バイト単位で入力してください。\n\n例:\n0x101010\n0x101014\n0x101018\n0x10101C\n0x101020")();
                        return;

                    } else if (Process::Read32(changeAddress, initValue)) {
                        hook->Initialize(changeAddress, targetAddress);
                        codeASM(targetAddress);
                        hook->Enable();
                        isCheck = true;
                        isHotkey = false;
                        breakPoint = false;
                    }
                // キーボードで0x100000以下ならやり直し
                } else if (changeAddress <= 0x100000) {
                    isFirstTime = true;
                    return;
                }
            
            // 再度ボタンを押したらフック停止して値を元に戻す、値が同じなら何もしない
            } else if (isCheck == true) {
                if (*(u32*)changeAddress == initValue) {
                    isFirstTime = true;
                    isCheck = false;
                    isHotkey = true;
                } else {
                    hook->Disable();
                    *(u32*)changeAddress = initValue;
                    isFirstTime = true;
                    isCheck = false;
                    isHotkey = true;
                    breakPoint = true;
                    // ProcessImpl::Play(breakPoint);
                    OSD::Notify((Color::Lime << "Initialization successful!"));
                }
            }
        }

        // フック停止した後にエントリーを無効にする処理
        if (isHotkey == true) {
            if (entry->Hotkeys[1].IsPressed()) {
                OSD::Notify((Color::Red << "Entry got nuked!"));
                isHotkey = false;
                entry->Disable();
            }
        }
        
        // // レジスタ出力
        regAddress = targetAddress + 0x50;
        // u32 posX = 301;
        u32 posX = 0;
        u32 posY = 0;

        do {
            u32 regValue = *(u32*)regAddress;

            if (posY / 10 >= 10 && posY / 10 <= 12) {
                topScr.Draw(Utils::Format("r%d  =  %08X", posY / 10, regValue) + "\0\n", posX, posY);
            } else {
                topScr.Draw(Utils::Format("r%d   =  %08X", posY / 10, regValue) + "\0\n", posX, posY);
            }

            regAddress += 4;
            posY += 10;

            if (posY > 120) {
                regValue = *(u32*)regAddress;
                topScr.Draw(Utils::Format("sp   =  %08X", regValue) + "\0\n", posX, posY);

                regAddress += 4;
                regValue = *(u32*)regAddress;
                posY += 10;
                topScr.Draw(Utils::Format("lr   =  %08X", regValue) + "\0\n", posX, posY);

                regAddress += 4;
                regValue = *(u32*)regAddress;
                posY += 10;
                topScr.Draw(Utils::Format("pc   =  %08X", regValue) + "\0\n", posX, posY);

                regAddress += 4;
                regValue = *(u32*)regAddress;
                posY += 10;
                topScr.Draw(Utils::Format("cpsr =  %08X", regValue) + "\0\n", posX, posY);
            }
        } while (posY <= 120);

    }
}