// TRPO.cpp
#include "UI.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    while (true) {
        system("cls");
        std::cout << " СИСТЕМА ПОИСКА ОДНОРОДНЫХ РИФМ (ПРИЧАСТИЯ)\n\n";
        std::cout << " 1. Запуск анализа текста\n";
        std::cout << " 2. Инструкция пользователя\n";
        std::cout << " 0. Выход из программы\n\n";
        std::cout << " Выберите действие: ";

        int choice = getSafeIntInput(0, 2);

        if (choice == 0) {
            std::cout << "\n Работа программы завершена.\n";
            break;
        }
        else if (choice == 2) {
            printInstructions("instructions.txt");
            std::cout << "Нажмите любую клавишу для возврата в меню...";
            _getch();
        }
        else if (choice == 1) {
            runAnalysisPipeline();
        }
    }

    return 0;
}