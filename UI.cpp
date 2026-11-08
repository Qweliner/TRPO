#include "UI.h"
#include "TextParser.h"
#include "ParticipleDetector.h"
#include "RhymeDetector.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <conio.h>

int getSafeIntInput(int minVal, int maxVal) {
    std::string line;
    while (true) {
        if (!std::getline(std::cin, line)) {
            return minVal;
        }

        // Если пользователь просто жмет Enter или пробелы — игнорируем без спама ошибками
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
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
        std::cout << "Некорректный ввод. Введите число от " << minVal << " до " << maxVal << ": ";
    }
}

bool isValidFilename(const std::string& name) {
    if (name.empty()) return false;
    return name.find_first_of("<>:\"/\\|?*") == std::string::npos;
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.is_open();
}

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

void printInstructions(const std::string& helpFile) {
    system("cls");
    std::ifstream file(helpFile.c_str());
    if (!file.is_open()) {
        std::cout << "Инструкция не найдена. Поместите " << helpFile << " рядом с программой.\n\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
    std::cout << "\n";
}

std::string selectSourceFileStep() {
    system("cls");
    std::cout << "ВЫБОР ВХОДНОГО ФАЙЛА:\n\n";
    std::cout << "1. test_onegin.txt (Текст поэмы с причастиями и пунктуацией)\n";
    std::cout << "2. test_poem.txt (Стихотворный тест с исключениями)\n";
    std::cout << "3. test_prose.txt (Проза без рифм)\n";
    std::cout << "4. test_trash.txt (Тест на мусорные символы и знаки препинания)\n";
    std::cout << "5. test_empty.txt (Пустой файл)\n";
    std::cout << "6. Ввести имя файла вручную\n";
    std::cout << "0. Назад в главное меню\n\n";
    std::cout << "Выберите пункт: ";

    int choice = getSafeIntInput(0, 6);
    if (choice == 0) return "";
    if (choice == 1) return "test_onegin.txt";
    if (choice == 2) return "test_poem.txt";
    if (choice == 3) return "test_prose.txt";
    if (choice == 4) return "test_trash.txt";
    if (choice == 5) return "test_empty.txt";

    std::cout << "\nВведите имя файла (без расширения .txt, 0 для отмены): ";
    std::string manualName;
    while (true) {
        std::getline(std::cin, manualName);
        size_t first = manualName.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            continue;
        }
        manualName = manualName.substr(first);

        if (manualName == "0") return "";

        if (!isValidFilename(manualName)) {
            std::cout << "Имя содержит недопустимые символы. Повторите ввод: ";
            continue;
        }

        std::string fullPath = manualName;
        if (fullPath.length() < 4 || fullPath.substr(fullPath.length() - 4) != ".txt") {
            fullPath += ".txt";
        }

        if (!fileExists(fullPath)) {
            std::cout << "Файл не найден в папке с программой. Повторите ввод: ";
            continue;
        }
        return fullPath;
    }
}

// Вход: имя файла, список предложений, найденные пары рифм и статистика.
// Что делает: сохраняет разметку предложений и отчет анализа в два выходных файла строго после подтверждения.
// Выход: нет.
void saveReportStep(const std::string& inputFilename,
    const std::vector<SentenceItem>& sentences,
    const std::vector<RhymePair>& pairs,
    const std::vector<WordStat>& stats) {
    std::string baseName = inputFilename;
    size_t dotPos = baseName.rfind(".txt");
    if (dotPos != std::string::npos) {
        baseName = baseName.substr(0, dotPos);
    }

    std::string reportBase = baseName + "_рифмы";
    std::string suggestedName = reportBase;
    if (fileExists(reportBase + ".txt")) {
        suggestedName = getIndexedFilename(reportBase);
        size_t sDot = suggestedName.rfind(".txt");
        if (sDot != std::string::npos) {
            suggestedName = suggestedName.substr(0, sDot);
        }
    }

    std::cout << "\nИмя для отчета по умолчанию: " << suggestedName << ".txt\n";
    std::cout << "Нажмите Enter для подтверждения или введите свое имя (без .txt): ";

    std::string userOut;
    std::getline(std::cin, userOut);
    size_t f = userOut.find_first_not_of(" \t\r\n");
    if (f == std::string::npos) {
        userOut = suggestedName;
    }
    else {
        userOut = userOut.substr(f);
    }

    std::string finalReportPath = userOut + ".txt";
    std::string finalSentencesPath = userOut + "_предложения.txt";

    // Проверка на перезапись: если существует хотя бы один из файлов
    if (fileExists(finalReportPath) || fileExists(finalSentencesPath)) {
        std::cout << "\n[ВНИМАНИЕ] Выходные файлы для \"" << userOut << "\" уже существуют!\n";
        std::cout << "Перезаписать существующие файлы? (Y/N): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (!(ans == "Y" || ans == "y" || ans == "Да" || ans == "да")) {
            std::cout << "Сохранение отменено пользователем.\n";
            return;
        }
    }

    // Сохранение файла с размеченными предложениями
    if (saveNumberedSentences(finalSentencesPath, sentences)) {
        std::cout << "Размеченные предложения сохранены: " << finalSentencesPath << "\n";
    }
    else {
        std::cout << "Ошибка при сохранении файла предложений.\n";
    }

    // Сохранение файла с рифмами
    std::ofstream out(finalReportPath.c_str());
    if (!out.is_open()) {
        std::cout << "Ошибка при создании файла отчета по рифмам.\n";
        return;
    }

    out << "Отчет анализа текста: поиск однородных рифм (причастия)\n";
    out << "Исходный файл: " << inputFilename << "\n";
    out << "Всего найдено рифмующихся пар: " << pairs.size() << "\n\n";

    if (!pairs.empty()) {
        out << "Список найденных рифмующихся пар:\n";
        for (size_t i = 0; i < pairs.size(); ++i) {
            out << (i + 1) << ". " << pairs[i].word1 << " (предл. " << pairs[i].sentenceId1
                << ") - " << pairs[i].word2 << " (предл. " << pairs[i].sentenceId2 << ")\n";
        }
        out << "\n";

        out << "Статистика слов по частоте вхождения в рифмы:\n";
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
    std::cout << "Отчет по рифмам сохранен: " << finalReportPath << "\n";
}

void runAnalysisPipeline() {
    std::string inputFile = selectSourceFileStep();
    if (inputFile.empty()) return;

    system("cls");
    std::cout << "Обработка файла...\n\n";

    // Проверка словарей №2 (непосредственно перед разбором текста)
    std::string missingDict;
    if (!checkDictionaryFiles(missingDict)) {
        std::cout << "Ошибка: не найден файл словаря: " << missingDict << "\n";
        std::cout << "Положите данный файл в рабочую папку программы.\n\n";
        std::cout << "Нажмите любую клавишу для возврата...";
        _getch();
        return;
    }

    // Загрузка словарей в память
    if (!loadDictionaries()) {
        std::cout << "Ошибка чтения файлов словарей.\n\n";
        std::cout << "Нажмите любую клавишу для возврата...";
        _getch();
        return;
    }

    std::vector<WordToken> participles;
    std::vector<SentenceItem> sentences;

    std::cout << "Чтение текста и разметка предложений: ";
    if (!parseTextFile(inputFile, participles, sentences)) {
        std::cout << "\nОшибка: не удалось прочитать входной файл.\n\n";
        std::cout << "Нажмите любую клавишу для возврата...";
        _getch();
        return;
    }

    std::cout << "\nВсего выделено предложений: " << sentences.size() << "\n";
    std::cout << "Найдено причастий: " << participles.size() << "\n";

    if (sentences.empty()) {
        std::cout << "Входной файл пуст.\n\n";
        std::cout << "Нажмите любую клавишу для возврата...";
        _getch();
        return;
    }

    std::cout << "Поиск созвучий: ";
    std::vector<RhymePair> pairs = findRhymePairs(participles);
    std::cout << "\nНайдено рифмующихся пар: " << pairs.size() << "\n";

    std::vector<WordStat> stats = buildStatistics(pairs);
    selectionSortStats(stats);

    saveReportStep(inputFile, sentences, pairs, stats);

    std::cout << "\nОбработка завершена. Нажмите любую клавишу для возврата...";
    _getch();
}