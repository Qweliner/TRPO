// TextParser.cpp
#include "TextParser.h"
#include "ParticipleDetector.h"
#include <iostream>
#include <fstream>

// Вход: символ в кодировке Windows-1251.
// Что делает: преобразует заглавные русские (192..223) и английские буквы в строчные.
// Выход: строчный символ.
char toLowerChar(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    // Английские буквы A-Z
    if (uc >= 'A' && uc <= 'Z') {
        return static_cast<char>(uc + ('a' - 'A'));
    }
    // Русские буквы А-Я (Windows-1251: 0xC0..0xDF)
    if (uc >= 192 && uc <= 223) {
        return static_cast<char>(uc + 32);
    }
    // Русская буква Ё (0xA8 -> 0xB8)
    if (uc == 168) {
        return static_cast<char>(184);
    }
    return c;
}

// Вход: символ.
// Что делает: проверяет пунктуацию конца предложения.
// Выход: признак разделителя предложений.
bool isSentenceDelimiter(char c) {
    return (c == '.' || c == '!' || c == '?');
}

// Вход: исходный токен из текста.
// Что делает: убирает пунктуацию по краям и приводит к нижнему регистру.
// Выход: очищенное слово.
std::string sanitizeWord(const std::string& raw) {
    if (raw.empty()) {
        return "";
    }

    size_t start = 0;
    while (start < raw.length()) {
        unsigned char uc = static_cast<unsigned char>(raw[start]);
        // Допустимы буквы (латиница и кириллица Windows-1251)
        bool isLetter = (uc >= 'a' && uc <= 'z') || (uc >= 'A' && uc <= 'Z') ||
            (uc >= 192 && uc <= 255) || (uc == 168) || (uc == 184);
        if (isLetter) break;
        start++;
    }

    if (start >= raw.length()) {
        return "";
    }

    size_t end = raw.length() - 1;
    while (end > start) {
        unsigned char uc = static_cast<unsigned char>(raw[end]);
        bool isLetter = (uc >= 'a' && uc <= 'z') || (uc >= 'A' && uc <= 'Z') ||
            (uc >= 192 && uc <= 255) || (uc == 168) || (uc == 184);
        if (isLetter) break;
        end--;
    }

    std::string result = "";
    for (size_t i = start; i <= end; ++i) {
        result += toLowerChar(raw[i]);
    }

    return result;
}

// Вход: имя файла и ссылка на флаг статуса.
// Что делает: потоково посимвольно читает текст, объединяет знаки многоточия (...) в один конец предложения,
//             печатает точку каждые 200 слов.
// Выход: вектор извлеченных причастий с номерами предложений.
std::vector<WordToken> extractParticiplesFromFile(const std::string& filename, bool& isOk) {
    std::vector<WordToken> participles;
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
        isOk = false;
        return participles;
    }

    isOk = true;
    int currentSentence = 1;
    long long wordCounter = 0;
    std::string currentToken = "";
    char ch = 0;
    bool inDelimiterSequence = false; // Флаг для подавления дублирующих точек в "..." или "?!"

    while (file.get(ch)) {
        // Проверка на знаки конца предложения (точка, восклицательный, вопросительный)
        if (isSentenceDelimiter(ch)) {
            // Если перед знаком копилось слово — завершаем его до перехода к следующему предложению
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);
                if (!clean.empty()) {
                    wordCounter++;
                    if (wordCounter % 200 == 0) {
                        std::cout << ".";
                    }
                    if (isParticiple(clean)) {
                        participles.push_back({ clean, currentSentence });
                    }
                }
                currentToken.clear();
            }

            // Увеличиваем номер предложения ТОЛЬКО на первом знаке из серии (например, на первой точке из трех)
            if (!inDelimiterSequence) {
                currentSentence++;
                inDelimiterSequence = true;
            }
            continue;
        }

        // Если пошел любой другой символ — серия знаков конца предложения завершена
        inDelimiterSequence = false;

        // Пробельные символы отделяют слова друг от друга
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);
                if (!clean.empty()) {
                    wordCounter++;
                    if (wordCounter % 200 == 0) {
                        std::cout << ".";
                    }
                    if (isParticiple(clean)) {
                        participles.push_back({ clean, currentSentence });
                    }
                }
                currentToken.clear();
            }
        }
        else {
            currentToken += ch;
        }
    }

    // Обработка последнего слова, если в конце файла не было точки
    if (!currentToken.empty()) {
        std::string clean = sanitizeWord(currentToken);
        if (!clean.empty() && isParticiple(clean)) {
            participles.push_back({ clean, currentSentence });
        }
    }

    file.close();
    return participles;
}