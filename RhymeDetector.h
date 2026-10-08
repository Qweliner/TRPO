#pragma once
// RhymeDetector.h
#pragma once
#include <string>
#include <vector>
#include "Types.h"

// Вход: два очищенных слова.
// Что делает: проверяет наличие созвучия между словами (однородная рифма причастий по одинаковому суффиксу и окончанию).
// Выход: true, если слова образуют рифмующуюся пару; false иначе.
bool isRhyme(const std::string& w1, const std::string& w2);

// Вход: массив найденных причастий и размер окна поиска (0 - весь текст, >0 - разница в номерах предложений).
// Что делает: находит все рифмующиеся пары причастий с учетом заданного окна и выводит индикацию точками.
// Выход: массив найденных пар RhymePair.
std::vector<RhymePair> findRhymePairs(const std::vector<WordToken>& tokens, int windowSize);

// Вход: список рифмующихся пар.
// Что делает: агрегирует статистику: считает частоту вхождения каждого слова в рифмы и формирует список уникальных предложений.
// Выход: массив структур WordStat.
std::vector<WordStat> buildStatistics(const std::vector<RhymePair>& pairs);

// Вход: массив статистики WordStat (по ссылке).
// Что делает: выполняет сортировку методом прямого выбора (Selection Sort) по убыванию частоты (при равенстве — по алфавиту).
// Выход: нет (модифицирует массив на месте).
void selectionSortStats(std::vector<WordStat>& stats);