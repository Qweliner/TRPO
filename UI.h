// UI.h
#pragma once
#include <string>
#include <vector>
#include "Types.h"
#include "TextParser.h"
#include "ParticipleDetector.h"
#include "RhymeDetector.h"

// Вход: границы диапазона допустимых значений [minVal, maxVal].
// Что делает: считывает строку, отсекает нечисловые символы и гарантирует ввод целого числа из диапазона.
// Выход: корректное число.
int getSafeIntInput(int minVal, int maxVal);

// Вход: имя файла.
// Что делает: проверяет строку на отсутствие запрещенных символов Windows (< > : " / \ | ? *).
// Выход: true, если имя допустимо; false иначе.
bool isValidFilename(const std::string& name);

// Вход: имя файла.
// Что делает: проверяет физическое существование файла на диске.
// Выход: true, если файл доступен для чтения; false иначе.
bool fileExists(const std::string& filename);

// Вход: базовое имя выходного файла.
// Что делает: формирует имя с индексом в скобках для исключения случайной перезаписи.
// Выход: свободное имя файла вида "Name(1).txt".
std::string getIndexedFilename(const std::string& baseName);

// Вход: путь к файлу справки.
// Что делает: считывает instructions.txt и выводит его в консоль.
// Выход: нет.
void printInstructions(const std::string& helpFile);

// Вход: нет.
// Что делает: отображает меню выбора входного файла (пресеты или ручной ввод) с защитой от ошибок.
// Выход: путь к выбранному файлу (или пустая строка при отмене).
std::string selectSourceFileStep();

// Вход: нет.
// Что делает: запрашивает размер окна поиска (0 - весь текст, N - предложения) с возможностью отмены (-1).
// Выход: размер окна.
int selectWindowSizeStep();

// Вход: параметры анализа, список пар и отсортированная статистика.
// Что делает: формирует авто-имя, решает вопрос о перезаписи (запрос 'Y' или индекс) и сохраняет отчет.
// Выход: нет.
void saveReportStep(const std::string& inputFilename, int windowSize,
    const std::vector<RhymePair>& pairs,
    const std::vector<WordStat>& stats);

// Вход: нет.
// Что делает: координирует сквозной пайплайн: выбор файла -> окно -> парсинг -> рифмы -> статистика -> сохранение.
// Выход: нет.
void runAnalysisPipeline();