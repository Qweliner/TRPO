#include "RhymeDetector.h"
#include <iostream>
#include <algorithm>

bool isRhyme(const std::string& w1, const std::string& w2) {
    // Тавтологические пары (одинаковые слова) не являются рифмой
    if (w1 == w2) {
        return false;
    }

    const size_t minSuffixLen = 3;
    if (w1.length() < minSuffixLen || w2.length() < minSuffixLen) {
        return false;
    }

    std::string tail1 = w1.substr(w1.length() - minSuffixLen);
    std::string tail2 = w2.substr(w2.length() - minSuffixLen);

    return (tail1 == tail2);
}

std::vector<RhymePair> findRhymePairs(const std::vector<WordToken>& tokens) {
    std::vector<RhymePair> pairs;
    if (tokens.size() < 2) {
        return pairs;
    }

    // Поиск рифмующихся пар по всему массиву токенов без ограничений на окно
    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = i + 1; j < tokens.size(); ++j) {
            if (isRhyme(tokens[i].word, tokens[j].word)) {
                RhymePair p;
                p.word1 = tokens[i].word;
                p.sentenceId1 = tokens[i].sentenceId;
                p.word2 = tokens[j].word;
                p.sentenceId2 = tokens[j].sentenceId;
                pairs.push_back(p);
            }
        }

        // Индикация процесса обработки каждые 50 шагов
        if ((i + 1) % 50 == 0) {
            std::cout << ".";
        }
    }

    return pairs;
}

// Вспомогательная функция добавления слова в статистику без дублирования номеров предложений
static void addWordToStats(std::vector<WordStat>& stats, const std::string& word, int sentenceId) {
    int foundIdx = -1;
    for (size_t i = 0; i < stats.size(); ++i) {
        if (stats[i].word == word) {
            foundIdx = static_cast<int>(i);
            break;
        }
    }

    if (foundIdx == -1) {
        WordStat newStat;
        newStat.word = word;
        newStat.countInPairs = 1;
        newStat.sentenceIds.push_back(sentenceId);
        stats.push_back(newStat);
    }
    else {
        stats[foundIdx].countInPairs++;
        // Добавляем номер предложения только в том случае, если его еще нет в списке
        if (std::find(stats[foundIdx].sentenceIds.begin(), stats[foundIdx].sentenceIds.end(), sentenceId) == stats[foundIdx].sentenceIds.end()) {
            stats[foundIdx].sentenceIds.push_back(sentenceId);
        }
    }
}

std::vector<WordStat> buildStatistics(const std::vector<RhymePair>& pairs) {
    std::vector<WordStat> stats;
    for (size_t i = 0; i < pairs.size(); ++i) {
        addWordToStats(stats, pairs[i].word1, pairs[i].sentenceId1);
        addWordToStats(stats, pairs[i].word2, pairs[i].sentenceId2);
    }
    return stats;
}

void selectionSortStats(std::vector<WordStat>& stats) {
    for (size_t i = 0; i < stats.size(); ++i) {
        size_t bestIdx = i;
        for (size_t j = i + 1; j < stats.size(); ++j) {
            // Сортировка по убыванию частоты вхождения
            if (stats[j].countInPairs > stats[bestIdx].countInPairs) {
                bestIdx = j;
            }
            // При равной частоте — упорядочивание по алфавиту
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