#pragma once
#include <string>
#include <vector>

// Структура токена найденного слова с привязкой к предложению
struct WordToken {
    std::string word;
    int sentenceId;
};

// Структура найденной пары рифмующихся слов
struct RhymePair {
    std::string word1;
    int sentenceId1;
    std::string word2;
    int sentenceId2;
};

// Структура для сбора итоговой частотной статистики по слову
struct WordStat {
    std::string word;
    int countInPairs;
    std::vector<int> sentenceIds;
};

// Структура для сохранения размеченного предложения
struct SentenceItem {
    int id;
    std::string text;
};