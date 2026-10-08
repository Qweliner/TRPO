// RhymeDetector.cpp
#include "RhymeDetector.h"
#include <iostream>
#include <cmath>
#include <algorithm>

// Вход: два слова.
// Что делает: сравнивает хвосты слов. Рифмой считаются неидентичные слова с совпадением от 3 последних букв (суффикс + окончание).
// Выход: результат проверки (true/false).
bool isRhyme(const std::string& w1, const std::string& w2) {
    // Одинаковые слова не считаются рифмой (тавтология)
    if (w1 == w2) {
        return false;
    }

    // Для причастий совпадение суффикса и окончания составляет минимум 3 буквы (например, "-ший", "-щий", "-мый")
    const size_t rhymeSuffixLen = 3;
    if (w1.length() < rhymeSuffixLen || w2.length() < rhymeSuffixLen) {
        return false;
    }

    std::string tail1 = w1.substr(w1.length() - rhymeSuffixLen);
    std::string tail2 = w2.substr(w2.length() - rhymeSuffixLen);

    return (tail1 == tail2);
}

// Вход: вектор токенов причастий и окно поиска.
// Что делает: перебирает пары и формирует список созвучий в пределах окна предложений.
// Выход: вектор пар рифм.
std::vector<RhymePair> findRhymePairs(const std::vector<WordToken>& tokens, int windowSize) {
    std::vector<RhymePair> pairs;
    if (tokens.size() < 2) {
        return pairs;
    }

    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = i + 1; j < tokens.size(); ++j) {
            // Если задано окно поиска, проверяем расстояние между предложениями
            if (windowSize > 0) {
                int dist = std::abs(tokens[j].sentenceId - tokens[i].sentenceId);
                if (dist > windowSize) {
                    continue;
                }
            }

            if (isRhyme(tokens[i].word, tokens[j].word)) {
                RhymePair p;
                p.word1 = tokens[i].word;
                p.sentenceId1 = tokens[i].sentenceId;
                p.word2 = tokens[j].word;
                p.sentenceId2 = tokens[j].sentenceId;
                pairs.push_back(p);
            }
        }

        // Индикация работы каждые 50 шагов внешнего цикла
        if ((i + 1) % 50 == 0) {
            std::cout << ".";
        }
    }

    return pairs;
}

// Вход: список пар рифм.
// Что делает: агрегирует частоту каждого слова и объединяет номера предложений без дубликатов.
// Выход: вектор статистики по словам.
std::vector<WordStat> buildStatistics(const std::vector<RhymePair>& pairs) {
    std::vector<WordStat> stats;

    for (size_t i = 0; i < pairs.size(); ++i) {
        const RhymePair& p = pairs[i];

        // Обработка word1
        int idx1 = -1;
        for (size_t k = 0; k < stats.size(); ++k) {
            if (stats[k].word == p.word1) {
                idx1 = static_cast<int>(k);
                break;
            }
        }
        if (idx1 == -1) {
            WordStat newStat;
            newStat.word = p.word1;
            newStat.countInPairs = 1;
            newStat.sentenceIds.push_back(p.sentenceId1);
            stats.push_back(newStat);
        }
        else {
            stats[idx1].countInPairs++;
            if (std::find(stats[idx1].sentenceIds.begin(), stats[idx1].sentenceIds.end(), p.sentenceId1) == stats[idx1].sentenceIds.end()) {
                stats[idx1].sentenceIds.push_back(p.sentenceId1);
            }
        }

        // Обработка word2
        int idx2 = -1;
        for (size_t k = 0; k < stats.size(); ++k) {
            if (stats[k].word == p.word2) {
                idx2 = static_cast<int>(k);
                break;
            }
        }
        if (idx2 == -1) {
            WordStat newStat;
            newStat.word = p.word2;
            newStat.countInPairs = 1;
            newStat.sentenceIds.push_back(p.sentenceId2);
            stats.push_back(newStat);
        }
        else {
            stats[idx2].countInPairs++;
            if (std::find(stats[idx2].sentenceIds.begin(), stats[idx2].sentenceIds.end(), p.sentenceId2) == stats[idx2].sentenceIds.end()) {
                stats[idx2].sentenceIds.push_back(p.sentenceId2);
            }
        }
    }

    return stats;
}

// Вход: массив структур WordStat.
// Что делает: сортирует методом прямого выбора (Selection Sort) по убыванию частоты вхождений.
// Выход: нет (модифицирует массив).
void selectionSortStats(std::vector<WordStat>& stats) {
    for (size_t i = 0; i < stats.size(); ++i) {
        size_t bestIdx = i;
        for (size_t j = i + 1; j < stats.size(); ++j) {
            // Критерий 1: по убыванию частоты
            if (stats[j].countInPairs > stats[bestIdx].countInPairs) {
                bestIdx = j;
            }
            // Критерий 2 (при равенстве): по алфавиту (возрастание)
            else if (stats[j].countInPairs == stats[bestIdx].countInPairs) {
                if (stats[j].word < stats[bestIdx].word) {
                    bestIdx = j;
                }
            }
        }
        if (bestIdx != i) {
            std::swap(stats[i], stats[bestIdx]);
        }
    }
}