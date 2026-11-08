#include "ParticipleDetector.h"
#include <fstream>
#include <algorithm>

// Внутренние хранилища словарей в оперативной памяти
static std::vector<std::string> g_suffixes;
static std::vector<std::string> g_shortParticiples;
static std::vector<std::string> g_exceptions;
static bool g_dictionariesLoaded = false;

// Вход: имя файла на диске.
// Что делает: проверяет доступность файла для чтения.
// Выход: true, если файл существует и открывается.
static bool fileExists(const std::string& filename) {
    std::ifstream f(filename.c_str());
    return f.is_open();
}

// Вход: строка-источник и суффикс.
// Что делает: сверяет окончание строки.
// Выход: результат проверки (true/false).
bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// Вход: строка для имени отсутствующего файла.
// Что делает: осуществляет проверку наличия файлов словарей.
// Выход: true, если все файлы найдены.
bool checkDictionaryFiles(std::string& missingFile) {
    if (!fileExists(FILE_SUFFIXES)) {
        missingFile = FILE_SUFFIXES;
        return false;
    }
    if (!fileExists(FILE_SHORT)) {
        missingFile = FILE_SHORT;
        return false;
    }
    if (!fileExists(FILE_EXCEPTIONS)) {
        missingFile = FILE_EXCEPTIONS;
        return false;
    }
    return true;
}

// Вход: имя файла и вектор для сохранения строк.
// Что делает: построчно считывает файл, пропуская пустые строки.
// Выход: true при успехе.
static bool readLinesToVector(const std::string& filename, std::vector<std::string>& dest) {
    dest.clear();
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        // Убираем возможный \r от Windows-переносов
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            dest.push_back(line);
        }
    }
    file.close();
    return true;
}

// Вход: нет.
// Что делает: загружает словари в память.
// Выход: true, если загрузка всех списков прошла успешно.
bool loadDictionaries() {
    if (!readLinesToVector(FILE_SUFFIXES, g_suffixes)) return false;
    if (!readLinesToVector(FILE_SHORT, g_shortParticiples)) return false;
    if (!readLinesToVector(FILE_EXCEPTIONS, g_exceptions)) return false;

    g_dictionariesLoaded = true;
    return true;
}

// Вход: проверочное слово в нижнем регистре.
// Что делает: сверяет слово со словарями (исключения, малые формы, суффиксы).
// Выход: true, если слово является причастием.
bool isParticiple(const std::string& word) {
    // В русском языке причастий короче 3 букв не бывает
    if (word.length() < 3) {
        return false;
    }

    if (!g_dictionariesLoaded) {
        if (!loadDictionaries()) {
            return false;
        }
    }

    // Проверка по словарю исключений (псевдопричастия и прилагательные)
    for (size_t i = 0; i < g_exceptions.size(); ++i) {
        if (word == g_exceptions[i]) {
            return false;
        }
    }

    // Слова длиной 3 и 4 буквы сверяем строго по списку малых причастий
    if (word.length() < 5) {
        for (size_t i = 0; i < g_shortParticiples.size(); ++i) {
            if (word == g_shortParticiples[i]) {
                return true;
            }
        }
        return false;
    }

    // Слова от 5 букв проверяем по набору типовых суффиксов и окончаний
    for (size_t i = 0; i < g_suffixes.size(); ++i) {
        if (endsWith(word, g_suffixes[i])) {
            return true;
        }
    }

    return false;
}