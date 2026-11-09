#pragma once
#include <string>
#include <vector>
#include "Types.h"

// Вход: имя файла на диске.
// Что делает: проверяет существование и доступность файла для чтения.
// Выход: true, если файл существует; false иначе.
bool fileExists(const std::string& filename);

// Вход: строка с предполагаемым именем файла.
// Что делает: проверяет отсутствие запрещенных файловой системой Windows символов.
// Выход: true, если имя допустимо; false иначе.
bool isValidFilename(const std::string& name);

// Вход: базовое имя выходного файла.
// Что делает: подбирает синхронный свободный индекс для пары файлов отчета и предложений.
// Выход: уникальное базовое имя вида "Name(1)".
std::string getUnifiedIndexedBase(const std::string& baseName);

// Вход: имя файла справки.
// Что делает: считывает instructions.txt и выводит его в консоль.
// Выход: нет.
void printInstructions(const std::string& helpFile);

// Вход: нет.
// Что делает: отображает меню выбора входного файла с контролем наличия пресетов и поддержкой ESC.
// Выход: путь к выбранному файлу (или пустая строка при возврате).
std::string selectSourceFileStep();

// Вход: имя входного файла, список предложений, пары рифм и частотная статистика.
// Что делает: запрашивает подтверждение имени и синхронно сохраняет два выходных файла.
// Выход: true при успешном сохранении; false при отмене пользователем.
bool saveReportStep(const std::string& inputFilename,
    const std::vector<SentenceItem>& sentences,
    const std::vector<RhymePair>& pairs,
    const std::vector<WordStat>& stats);

// Вход: нет.
// Что делает: сквозное управление сценарием обработки текста с повторной проверкой словарей.
// Выход: нет.
void runAnalysisPipeline();