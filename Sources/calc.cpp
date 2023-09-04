#include "cheats.hpp"

namespace CTRPluginFramework {
    void calcHex(MenuEntry* entry) {
        static u32 number1 = 0;
        static u32 number2 = 0;
        static u32 answer = 0;
        static bool calcAdd;
        static bool calcSub;
        static bool calcMul;
        static bool calcDiv;

        std::vector<std::string> arithmetic {
            "+",
            "-",
            "×",
            "÷",
        };

        Keyboard kb1("最初に16進数を入力してください。");
        Keyboard kb2("次に算術演算子を入力してください。", arithmetic);
        Keyboard kb3("最後に16進数を入力してください。");
        kb1.IsHexadecimal(true);
        kb2.IsHexadecimal(true);
        kb3.IsHexadecimal(true);

        if (kb1.Open(number1, number1) != 0)
            return;

        const int kChoice = kb2.Open();
        switch (kChoice)
        {
        case 0:
            calcAdd = true;
            break;

        case 1:
            calcSub = true;
            break;

        case 2:
            calcMul = true;
            break;

        case 3:
            calcDiv = true;
            break;
    
        default:
            return;
        }

        if (calcAdd) {
            if (kb3.Open(number2, number2) != 0) 
                return;

            answer = number1 + number2;
            if (answer != 0) {
                Sleep(Milliseconds(3));
                MessageBox("計算結果: "+Utils::Format("%X" ,answer))();
            }
            calcAdd = false;
            return;
            
        } else if (calcSub) {
            if (kb3.Open(number2, number2) != 0) 
                return;

            answer = number1 - number2;
            if (answer != 0) {
                Sleep(Milliseconds(3));
                MessageBox("計算結果: "+Utils::Format("%X" ,answer))();
            }
            calcSub = false;
            return;

        } else if (calcMul) {
            if (kb3.Open(number2, number2) != 0) 
                return;

            answer = number1 * number2;
            if (answer != 0) {
                Sleep(Milliseconds(3));
                MessageBox("計算結果: "+Utils::Format("%X" ,answer))();
            }
            calcMul = false;
            return;

        } else if (calcDiv) {
            if (kb3.Open(number2, number2) != 0) 
                return;

            answer = number1 / number2;
            if (answer != 0) {
                Sleep(Milliseconds(3));
                MessageBox("計算結果: "+Utils::Format("%X" ,answer))();
            }
            calcDiv = false;
            return;
        }

        int num = 1;
    }
}