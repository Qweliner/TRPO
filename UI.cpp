// UI.cpp
#include "UI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <conio.h>

// Вход: минимальное и максимальное допустимое значение.
// Что делает: циклический ввод числа с проверкой диапазона и фильтрацией мусора.
// Выход: проверенное число.
int getSafeIntInput(int minVal, int maxVal) {
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line.empty()) {
            std::cout << " Ввод не может быть пустым. Повторите: ";
            continue;
        }

        std::stringstream ss(line);
        int val = 0;
        char leftover = 0;
        if ((ss >> val) && !(ss >> leftover)) {
            if (val >= minVal && val <= maxVal) {
                return val;
            }
        }
        std::cout << " Ошибка: некорректное значение. Введите число от " << minVal << " до " << maxVal << ": ";
    }
}

// Вход: имя файла.
// Что делает: проверяет строку на отсутствие запрещенных символов Windows.
// Выход: true, если имя корректно.
bool isValidFilename(const std::string& name) {
    if (name.empty()) return false;
    return name.find_first_of("<>:\"/\\|?*") == std::string::npos;
}

// Вход: имя файла.
// Что делает: пытается открыть файл для чтения, проверяя его наличие на диске.
// Выход: true, если файл существует.
bool fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.is_open();
}

// Вход: базовое имя без расширения.
// Что делает: генерирует свободное имя с числовым индексом в скобках.
// Выход: уникальное имя файла.
std::string getIndexedFilename(const std::string& baseName) {
    int index = 1;
    while (true) {
        std::string test = baseName + "(" + std::to_string(index) + ").txt";
        if (!fileExists(test)) {
            return test;
        }
        index++;
    }
}

// Вход: путь к файлу справки.
// Что делает: читает файл справки и выводит текст пользователю.
// Выход: нет.
void printInstructions(const std::string& helpFile) {
    system("cls");
    std::ifstream file(helpFile.c_str());
    if (!file.is_open()) {
        std::cout << "\n ! Инструкция не найдена. Положите " << helpFile << " в ту же папку, что и исполняемая программа.\n";
        return;
    }
    std::cout << "\n";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << " " << line << "\n";
    }
    std::cout << "\n";
}

// Вход: нет.
// Что делает: отображает меню выбора входного файла (пресеты или ручной ввод).
// Выход: имя файла или пустая строка при отмене.
std::string selectSourceFileStep() {
    system("cls");
    std::cout << " ВЫБОР ВХОДНОГО ФАЙЛА:\n\n";
    std::cout << " 1. test_poem.txt (Стих с причастиями и деепричастием-ловушкой)\n";
    std::cout << " 2. test_mayakovsky.txt (Лесенка Маяковского, тест на пробелы)\n";
    std::cout << " 3. test_prose.txt (Проза без рифм)\n";
    std::cout << " 4. test_empty.txt (Пустой файл)\n";
    std::cout << " 5. Ввести имя файла вручную\n";
    std::cout << " 0. Назад в главное меню\n\n";
    std::cout << " Выберите пункт: ";

    int choice = getSafeIntInput(0, 5);
    if (choice == 0) return "";
    if (choice == 1) return "test_poem.txt";
    if (choice == 2) return "test_mayakovsky.txt";
    if (choice == 3) return "test_prose.txt";
    if (choice == 4) return "test_empty.txt";

    std::cout << "\n ! Вводите только название (расширение .txt добавится автоматически, 0 - отмена).\n";
    std::cout << " Имя входного файла: ";
    std::string manualName;
    while (true) {
        std::getline(std::cin, manualName);
        if (manualName == "0") return "";
        if (!isValidFilename(manualName)) {
            std::cout << " Ошибка: имя содержит запрещенные символы (< > : \" / \\ | ? *). Повторите: ";
            continue;
        }

        std::string fullPath = manualName;
        if (fullPath.length() < 4 || fullPath.substr(fullPath.length() - 4) != ".txt") {
            fullPath += ".txt";
        }

        if (!fileExists(fullPath)) {
            std::cout << " Ошибка: файл не найден. Убедитесь, что файл лежит рядом с программой. Повторите: ";
            continue;
        }
        return fullPath;
    }
}

// Вход: нет.
// Что делает: диалог настройки окна предложений с возможностью отмены.
// Выход: размер окна (-1 - весь текст, 1..50 - расстояние, 0 - возврат).
int selectWindowSizeStep() {
    system("cls");
    std::cout << " НАСТРОЙКА ОКНА ПОИСКА РИФМ:\n\n";
    std::cout << " -1. Поиск по всему тексту (без ограничений)\n";
    std::cout << "  1. В пределах одного предложения\n";
    std::cout << "  2..50. В заданных пределах (расстояние между предложениями)\n";
    std::cout << "  0. Вернуться обратно в главное меню\n\n";
    std::cout << " Введите размер окна: ";

    return getSafeIntInput(-1, 50);
}

// Вход: имя файла, параметры, пары и отсортированная статистика.
// Что делает: сохраняет результат с подтверждением и авто-индексацией по логике автора.
// Выход: нет.
void saveReportStep(const std::string& inputFilename, int windowSize,
    const std::vector<RhymePair>& pairs,
    const std::vector<WordStat>& stats) {
    std::string baseOut = inputFilename;
    size_t dotPos = baseOut.rfind(".txt");
    if (dotPos != std::string::npos) {
        baseOut = baseOut.substr(0, dotPos);
    }
    std::string autoBase = baseOut + "_рифмы";
    std::string suggested = autoBase;

    std::cout << "\n СОХРАНЕНИЕ РЕЗУЛЬТАТОВ:\n";
    if (fileExists(autoBase + ".txt")) {
        suggested = getIndexedFilename(autoBase);
        size_t sDot = suggested.rfind(".txt");
        if (sDot != std::string::npos) suggested = suggested.substr(0, sDot);

        std::cout << " Предупреждение: базовый файл " << autoBase << ".txt уже существует.\n";
    }

    std::cout << " Автоматическое имя: " << suggested << ".txt\n";
    std::cout << " Нажмите Enter, чтобы применить автоназвание,\n";
    std::cout << " или введите свое имя (без .txt): ";

    std::string userOut;
    std::getline(std::cin, userOut);
    if (userOut.empty()) {
        userOut = suggested;
    }

    std::string finalPath = userOut + ".txt";

    // Если пользователь вручную ввел уже существующий файл — запрашиваем подтверждение
    if (fileExists(finalPath)) {
        std::cout << "\n [ВНИМАНИЕ] Файл " << finalPath << " уже существует!\n";
        std::cout << " Перезаписать? (Y - да, любой другой ввод - отмена сохранения): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (!(ans == "Y" || ans == "y" || ans == "Да" || ans == "да")) {
            std::cout << " Сохранение отменено пользователем.\n";
            return;
        }
    }

    std::ofstream out(finalPath.c_str());
    if (!out.is_open()) {
        std::cout << " Ошибка: не удалось создать файл. Проверьте права на запись в папку.\n";
        return;
    }

    out << "Отчет поиска однородных рифм (причастия)\n";
    out << "Исходный файл: " << inputFilename << "\n";
    out << "Окно поиска: " << (windowSize <= 0 ? "весь текст" : std::to_string(windowSize) + " предл.") << "\n";
    out << "Всего найдено рифмующихся пар: " << pairs.size() << "\n\n";

    if (!pairs.empty()) {
        out << "Найденные рифмующиеся пары:\n";
        for (size_t i = 0; i < pairs.size(); ++i) {
            out << i + 1 << ". " << pairs[i].word1 << " (предл. " << pairs[i].sentenceId1
                << ") - " << pairs[i].word2 << " (предл. " << pairs[i].sentenceId2 << ")\n";
        }
        out << "\n";

        out << "Упорядоченный список слов по частоте в парах:\n";
        for (size_t i = 0; i < stats.size(); ++i) {
            out << stats[i].word << " (вхождений: " << stats[i].countInPairs << ", предложения: ";
            for (size_t s = 0; s < stats[i].sentenceIds.size(); ++s) {
                out << stats[i].sentenceIds[s] << (s + 1 < stats[i].sentenceIds.size() ? ", " : "");
            }
            out << ")\n";
        }
    }
    else {
        out << "Рифмующихся пар причастий в тексте не обнаружено.\n";
    }

    out.close();
    std::cout << "\n Запись отсортированных данных успешно завершена: " << finalPath << "\n";
}

// Вход: нет.
// Что делает: сквозное управление сценарием обработки.
// Выход: нет.
void runAnalysisPipeline() {
    std::string inputFile = selectSourceFileStep();
    if (inputFile.empty()) return; // Мгновенный выход в меню без пауз!

    int windowSize = selectWindowSizeStep();
    if (windowSize == 0) return;   // Мгновенный выход в меню без пауз!

    system("cls");
    std::cout << " ВЫПОЛНЕНИЕ АНАЛИЗА ДАННЫХ:\n\n";
    std::cout << " Чтение файла и поиск причастий: ";
    bool isOk = false;
    std::vector<WordToken> participles = extractParticiplesFromFile(inputFile, isOk);

    if (!isOk) {
        std::cout << "\n Ошибка: не удалось открыть файл. Проверьте целостность файла.\n";
        std::cout << "\n Нажмите любую клавишу для возврата в меню...";
        _getch();
        return;
    }
    std::cout << "\n Найдено причастий в тексте: " << participles.size() << "\n";

    if (participles.empty()) {
        std::cout << " Причастия в тексте не обнаружены.\n\n";
        std::cout << " Сохранить пустой отчет в файл? (Y - да, любая другая клавиша - нет): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans == "Y" || ans == "y" || ans == "Да" || ans == "да") {
            saveReportStep(inputFile, windowSize, {}, {});
        }
        std::cout << "\n Нажмите любую клавишу для возврата в меню...";
        _getch();
        return;
    }

    std::cout << " Поиск рифмующихся пар: ";
    int effWindow = (windowSize == -1) ? 0 : windowSize;
    std::vector<RhymePair> pairs = findRhymePairs(participles, effWindow);
    std::cout << "\n Найдено рифмующихся пар: " << pairs.size() << "\n";

    if (pairs.empty()) {
        std::cout << " Рифмующихся пар не найдено.\n\n";
        std::cout << " Сохранить пустой отчет в файл? (Y - да, любая другая клавиша - нет): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans == "Y" || ans == "y" || ans == "Да" || ans == "да") {
            saveReportStep(inputFile, windowSize, {}, {});
        }
        std::cout << "\n Нажмите любую клавишу для возврата в меню...";
        _getch();
        return;
    }

    std::vector<WordStat> stats = buildStatistics(pairs);
    selectionSortStats(stats);

    saveReportStep(inputFile, windowSize, pairs, stats);

    // Пауза только в самом конце, когда работа РЕАЛЬНО сделана
    std::cout << "\n Нажмите любую клавишу для возврата в главное меню...";
    _getch();
}