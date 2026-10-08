// Types.h
#pragma once
#include <string>
#include <vector>

// Структура для хранения найденного слова и номера предложения, где оно встретилось
struct WordToken {
    std::string word;
    int sentenceId = 0;
};

// Структура для хранения рифмующейся пары и координат предложений
struct RhymePair {
    std::string word1;
    int sentenceId1 = 0;
    std::string word2;
    int sentenceId2 = 0;
};

// Структура для итоговой статистики по каждому слову, вошедшему в пары
struct WordStat {
    std::string word;
    int countInPairs = 0;
    std::vector<int> sentenceIds;
};