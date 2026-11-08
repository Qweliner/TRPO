#include "TextParser.h"
#include "ParticipleDetector.h"
#include <fstream>
#include <iostream>

bool isRussianLetter(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= 192 && uc <= 255) || (uc == 168) || (uc == 184);
}

char toLowerRus(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    if (uc >= 192 && uc <= 223) {
        return static_cast<char>(uc + 32);
    }
    if (uc == 168) {
        return static_cast<char>(184);
    }
    return c;
}

bool isSentenceDelimiter(char c) {
    return (c == '.' || c == '!' || c == '?');
}

std::string sanitizeWord(const std::string& raw) {
    if (raw.empty()) return "";

    size_t start = 0;
    while (start < raw.length() && !isRussianLetter(raw[start])) {
        start++;
    }
    if (start >= raw.length()) return "";

    size_t end = raw.length() - 1;
    while (end > start && !isRussianLetter(raw[end])) {
        end--;
    }

    std::string result = "";
    for (size_t i = start; i <= end; ++i) {
        result += toLowerRus(raw[i]);
    }
    return result;
}

// ¬спомогательна€ очистка лишних пробелов в начале и конце предложени€
static std::string trimString(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool parseTextFile(const std::string& filename,
    std::vector<WordToken>& participles,
    std::vector<SentenceItem>& sentences) {
    participles.clear();
    sentences.clear();

    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }

    int currentSentenceId = 1;
    long long wordCounter = 0;
    std::string currentToken = "";
    std::string currentSentenceText = "";
    bool inDelimiterSequence = false;
    char ch = 0;

    while (file.get(ch)) {
        // «амен€ем переносы строк на обычный пробел дл€ сохранени€ единой строки предложени€
        if (ch == '\n' || ch == '\r') {
            if (!currentSentenceText.empty() && currentSentenceText.back() != ' ') {
                currentSentenceText += ' ';
            }
        }
        else {
            currentSentenceText += ch;
        }

        if (isSentenceDelimiter(ch)) {
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);
                if (!clean.empty()) {
                    wordCounter++;
                    if (wordCounter % 200 == 0) {
                        std::cout << ".";
                    }
                    if (isParticiple(clean)) {
                        participles.push_back({ clean, currentSentenceId });
                    }
                }
                currentToken.clear();
            }

            // ќбработка серий знаков вроде "..." или "?!"
            if (!inDelimiterSequence) {
                inDelimiterSequence = true;
            }
            continue;
        }

        // ≈сли вышли из серии завершающих знаков препинани€ Ч закрываем текущее предложение
        if (inDelimiterSequence) {
            std::string trimmed = trimString(currentSentenceText);
            if (!trimmed.empty()) {
                sentences.push_back({ currentSentenceId, trimmed });
                currentSentenceId++;
            }
            currentSentenceText.clear();
            inDelimiterSequence = false;

            // “екущий символ уже относитс€ к следующему предложению
            if (ch != '\n' && ch != '\r') {
                currentSentenceText += ch;
            }
        }

        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);
                if (!clean.empty()) {
                    wordCounter++;
                    if (wordCounter % 200 == 0) {
                        std::cout << ".";
                    }
                    if (isParticiple(clean)) {
                        participles.push_back({ clean, currentSentenceId });
                    }
                }
                currentToken.clear();
            }
        }
        else {
            currentToken += ch;
        }
    }

    // ќбработка остатка слова и предложени€ при достижении конца файла
    if (!currentToken.empty()) {
        std::string clean = sanitizeWord(currentToken);
        if (!clean.empty() && isParticiple(clean)) {
            participles.push_back({ clean, currentSentenceId });
        }
    }

    std::string trimmed = trimString(currentSentenceText);
    if (!trimmed.empty()) {
        sentences.push_back({ currentSentenceId, trimmed });
    }

    file.close();
    return true;
}

bool saveNumberedSentences(const std::string& filename, const std::vector<SentenceItem>& sentences) {
    std::ofstream out(filename.c_str());
    if (!out.is_open()) {
        return false;
    }

    for (size_t i = 0; i < sentences.size(); ++i) {
        out << sentences[i].id << ". " << sentences[i].text << "\n";
    }

    out.close();
    return true;
}