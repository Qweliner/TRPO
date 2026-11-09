#include "UI.h"
#include "TextParser.h"
#include "ParticipleDetector.h"
#include "RhymeDetector.h"
#include <iostream>
#include <fstream>
#include <conio.h>

// Вход: строка приглашения, строка результата по ссылке и дефолтное значение.
// Что делает: безопасный ввод с фильтрацией стрелочек, поддержкой кириллицы 1251, буквы 'а' и отменой по ESC.
// Выход: true при нажатии Enter; false при нажатии ESC.
static bool safeInputLine(const std::string& prompt, std::string& resultStr, const std::string& defaultVal = "") {
    std::cout << prompt;
    resultStr.clear();

    while (true) {
        int key = _getch();

        // Перехват стрелок и спецклавиш: в Windows они шлют префикс 0 или 224, за которым сразу идет скан-код
        // Буква 'а' в CP1251 имеет код 224, но при её вводе в буфере нет второго байта (_kbhit() == false)
        if (key == 0 || key == 224) {
            if (_kbhit()) {
                _getch(); // Проглатываем скан-код стрелки (коды 72, 75, 77, 80 не попадут в строку)
                continue;
            }
        }

        // ESC - отмена ввода
        if (key == 27) {
            return false;
        }

        // Enter - подтверждение
        if (key == 13) {
            if (resultStr.empty() && !defaultVal.empty()) {
                resultStr = defaultVal;
            }
            if (!resultStr.empty()) {
                std::cout << "\n";
                return true;
            }
            continue; // Пустой ввод без дефолта просто ждем
        }

        // Backspace
        if (key == 8) {
            if (!resultStr.empty()) {
                resultStr.pop_back();
                std::cout << "\b \b";
            }
            continue;
        }

        // Печатаемые символы (включая русскую кодировку 1251: коды 192..255)
        unsigned char uc = static_cast<unsigned char>(key);
        if (uc >= 32) {
            char c = static_cast<char>(uc);
            // Запрещенные для файлов Windows символы
            if (std::string("<>:\"/\\|?*").find(c) == std::string::npos) {
                resultStr += c;
                std::cout << c;
            }
        }
    }
}

// Вход: имя файла на диске.
// Что делает: проверяет существование и доступность файла для чтения.
// Выход: true, если файл существует; false иначе.
bool fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.is_open();
}

// Вход: строка с предполагаемым именем файла.
// Что делает: проверяет отсутствие запрещенных файловой системой Windows символов.
// Выход: true, если имя допустимо; false иначе.
bool isValidFilename(const std::string& name) {
    if (name.empty()) return false;
    return name.find_first_of("<>:\"/\\|?*") == std::string::npos;
}

// Вход: базовое имя выходного файла.
// Что делает: подбирает синхронный свободный индекс для пары файлов отчета и предложений.
// Выход: уникальное базовое имя вида "Name(1)".
std::string getUnifiedIndexedBase(const std::string& baseName) {
    std::string testRhymes = baseName + "_рифмы.txt";
    std::string testSentences = baseName + "_предложения.txt";

    if (!fileExists(testRhymes) && !fileExists(testSentences)) {
        return baseName;
    }

    int index = 1;
    while (true) {
        std::string candBase = baseName + "(" + std::to_string(index) + ")";
        std::string candRhymes = candBase + "_рифмы.txt";
        std::string candSentences = candBase + "_предложения.txt";

        if (!fileExists(candRhymes) && !fileExists(candSentences)) {
            return candBase;
        }
        index++;
    }
}

// Вход: имя файла справки.
// Что делает: считывает instructions.txt и выводит его в консоль.
// Выход: нет.
void printInstructions(const std::string& helpFile) {
    system("cls");
    std::cout << "ESC: Назад в главное меню\n\n";
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

// Вход: нет.
// Что делает: отображает меню выбора входного файла с контролем наличия пресетов и поддержкой ESC.
// Выход: путь к выбранному файлу (или пустая строка при возврате).
std::string selectSourceFileStep() {
    const std::vector<std::string> presets = {
        "test_onegin.txt",
        "test_poem.txt",
        "test_mayakovsky.txt",
        "test_prose.txt",
        "test_trash.txt",
        "test_empty.txt"
    };

    while (true) {
        system("cls");
        std::cout << "ESC: Назад в главное меню\n\n";
        std::cout << "ВЫБОР ВХОДНОГО ФАЙЛА:\n\n";

        std::vector<bool> presetExists(presets.size(), false);
        for (size_t i = 0; i < presets.size(); ++i) {
            presetExists[i] = fileExists(presets[i]);
            std::cout << (i + 1) << ". " << presets[i];
            if (!presetExists[i]) {
                std::cout << " [файл не найден]";
            }
            std::cout << "\n";
        }
        std::cout << "7. Ввести имя файла вручную\n\n";
        std::cout << "Выберите пункт: ";

        int key = _getch();
        if (key == 27) {
            return "";
        }

        if (key >= '1' && key <= '6') {
            size_t idx = key - '1';
            if (presetExists[idx]) {
                return presets[idx];
            }
            else {
                std::cout << "\nФайл " << presets[idx] << " отсутствует в папке с программой.\n";
                std::cout << "Нажмите любую клавишу...";
                _getch();
                continue;
            }
        }

        if (key == '7') {
            std::cout << "7\n\nESC: Отмена ";
            std::string manualName;
            if (!safeInputLine("Введите имя файла (без .txt): ", manualName)) {
                continue; // По ESC вернулись к списку файлов
            }

            std::string fullPath = manualName + ".txt";
            if (!fileExists(fullPath)) {
                std::cout << "Файл \"" << fullPath << "\" не найден в рабочей папке.\n";
                std::cout << "Нажмите любую клавишу для повторного выбора...";
                _getch();
                continue;
            }
            return fullPath;
        }
    }
}

// Вход: имя входного файла, список предложений, пары рифм и частотная статистика.
// Что делает: запрашивает подтверждение имени и синхронно сохраняет два выходных файла.
// Выход: true при успешном сохранении; false при отмене пользователем.
bool saveReportStep(const std::string& inputFilename,
    const std::vector<SentenceItem>& sentences,
    const std::vector<RhymePair>& pairs,
    const std::vector<WordStat>& stats) {
    system("cls");
    std::cout << "ESC: Назад в главное меню\n\n";

    std::string baseName = inputFilename;
    size_t dotPos = baseName.rfind(".txt");
    if (dotPos != std::string::npos) {
        baseName = baseName.substr(0, dotPos);
    }

    std::string suggestedBase = getUnifiedIndexedBase(baseName);

    std::cout << "По результатам анализа будут сформированы два файла:\n";
    std::cout << "  1) " << suggestedBase << "_рифмы.txt       - пары созвучий и статистика\n";
    std::cout << "  2) " << suggestedBase << "_предложения.txt - размеченные предложения\n\n";

    std::cout << "Базовое имя по умолчанию: " << suggestedBase << "\n";
    std::string userBase;
    if (!safeInputLine("Нажмите Enter для подтверждения или введите свое базовое имя: ", userBase, suggestedBase)) {
        std::cout << "\nСохранение отменено.\n";
        return false;
    }

    std::string finalReportPath = userBase + "_рифмы.txt";
    std::string finalSentencesPath = userBase + "_предложения.txt";

    if (fileExists(finalReportPath) || fileExists(finalSentencesPath)) {
        std::cout << "\n[ВНИМАНИЕ] Файлы для \"" << userBase << "\" уже существуют!\n";
        std::cout << "Перезаписать существующие файлы? (Y - да, любая другая клавиша - отмена): ";
        char ans = _getch();
        std::cout << ans << "\n";
        if (ans != 'Y' && ans != 'y' && ans != 'Д' && ans != 'д') {
            std::cout << "Сохранение отменено пользователем.\n";
            return false;
        }
    }

    if (saveNumberedSentences(finalSentencesPath, sentences)) {
        std::cout << "Размеченные предложения сохранены: " << finalSentencesPath << "\n";
    }

    std::ofstream out(finalReportPath.c_str());
    if (!out.is_open()) {
        std::cout << "Ошибка при создании файла отчета.\n";
        return false;
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
    return true;
}

// Вход: нет.
// Что делает: сквозное управление сценарием обработки текста с повторной проверкой словарей.
// Выход: нет.
void runAnalysisPipeline() {
    std::string inputFile = selectSourceFileStep();
    if (inputFile.empty()) return;

    system("cls");
    std::cout << "ESC: Отмена\n\n";
    std::cout << "Обработка файла...\n\n";

    std::string missingDict;
    if (!checkDictionaryFiles(missingDict)) {
        std::cout << "Ошибка: не найден файл словаря: " << missingDict << "\n";
        std::cout << "Положите данный файл в рабочую папку программы.\n\n";
        std::cout << "Нажмите любую клавишу для возврата...";
        _getch();
        return;
    }

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
    std::cout << "\nПоиск завершен. Нажмите любую клавишу для продолжения.";
    _getch();

    std::vector<WordStat> stats = buildStatistics(pairs);
    selectionSortStats(stats);

    saveReportStep(inputFile, sentences, pairs, stats);

    std::cout << "\nОбработка завершена. Нажмите любую клавишу для возврата...";
    _getch();
    return;
}