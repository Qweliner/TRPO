#include "UI.h"
#include "ParticipleDetector.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

// Вход: нет.
// Что делает: настраивает локаль консоли (кодовая страница 1251) и организует главный цикл программы.
// Выход: код возврата 0 при штатном завершении.
int main() {
    // Установка русской кодировки для работы с кириллицей Windows-1251
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    while (true) {
        system("cls");
        std::cout << "Программа поиска причастий и однородных рифм в тексте\n\n";

        // Проверка №1: контроль доступности словарей при запуске и возврате в меню
        std::string missingFile;
        if (!checkDictionaryFiles(missingFile)) {
            std::cout << "Предупреждение: отсутствует словарь: " << missingFile << "\n";
            std::cout << "Для анализа поместите файл в папку с исполняемой программой.\n\n";
        }

        std::cout << "1. Запуск анализа текстового файла\n";
        std::cout << "2. Инструкция пользователя\n";
        std::cout << "0. Выход\n\n";
        std::cout << "Выберите действие: ";

        int choice = getSafeIntInput(0, 2);

        if (choice == 0) {
            std::cout << "\nЗавершение работы программы.\n";
            break;
        }
        else if (choice == 2) {
            printInstructions("instructions.txt");
            std::cout << "Нажмите любую клавишу для возврата в главное меню...";
            _getch();
        }
        else if (choice == 1) {
            runAnalysisPipeline();
        }
    }

    return 0;
}