#pragma once
#include <string>
#include <vector>
#include "Types.h"

// Вход: два очищенных слова в нижнем регистре.
// Что делает: проверяет наличие созвучия (совпадение хвоста от 3 букв, исключая одинаковые слова).
// Выход: true, если слова образуют пару рифм; false иначе.
bool isRhyme(const std::string& w1, const std::string& w2);

// Вход: вектор найденных токенов причастий со всего текста.
// Что делает: находит все рифмующиеся пары причастий в пределах всего текста без ограничений по окну.
// Выход: вектор пар RhymePair.
std::vector<RhymePair> findRhymePairs(const std::vector<WordToken>& tokens);

// Вход: список найденных пар рифм.
// Что делает: подсчитывает количество вхождений каждого слова в пары и формирует список уникальных предложений.
// Выход: вектор структур WordStat.
std::vector<WordStat> buildStatistics(const std::vector<RhymePair>& pairs);

// Вход: вектор статистики WordStat по ссылке.
// Что делает: выполняет сортировку методом прямого выбора (Selection Sort) по убыванию частоты и алфавиту.
// Выход: нет (модифицирует переданный массив на месте).
void selectionSortStats(std::vector<WordStat>& stats);