#include "UI.h"
#include "ParticipleDetector.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

// Вход: нет.
// Что делает: настраивает кодовую страницу 1251, контролирует наличие словарей и запускает главное меню через _getch().
// Выход: код 0 при штатном завершении.
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    while (true) {
        system("cls");
        std::cout << "ESC: Выход из программы\n\n";
        std::cout << "ПРОГРАММА ПОИСКА ПРИЧАСТИЙ И ОДНОРОДНЫХ РИФМ\n\n";

        // Проверка №1: контроль доступности словарей при входе в меню
        std::string missingDict;
        if (!checkDictionaryFiles(missingDict)) {
            std::cout << "Не найден обязательный словарь: " << missingDict << "\n";
            std::cout << "Для анализа поместите файл в папку с исполняемой программой.\n\n";
        }

        std::cout << "1. Запуск анализа текстового файла\n";
        std::cout << "2. Инструкция пользователя\n\n";
        std::cout << "Выберите действие: ";

        int key = _getch();

        if (key == 27) {
            std::cout << "\nРабота программы завершена.\n";
            break;
        }

        if (key == '1') {
            runAnalysisPipeline();
        }
        else if (key == '2') {
            printInstructions("instructions.txt");
            std::cout << "Нажмите любую клавишу для возврата в главное меню...";
            _getch();
        }
    }

    return 0;
}