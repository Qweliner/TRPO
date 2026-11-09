#include "TextParser.h"
#include "ParticipleDetector.h"
#include <fstream>
#include <iostream>

// Вход: символ в кодировке Windows-1251.
// Что делает: проверяет принадлежность к русскому алфавиту.
// Выход: true, если символ русский; false иначе.
bool isRussianLetter(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= 192 && uc <= 255) || (uc == 168) || (uc == 184);
}

// Вход: символ в кодировке Windows-1251.
// Что делает: переводит заглавную русскую букву в строчную.
// Выход: строчный русский символ.
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

// Вход: символ текста.
// Что делает: проверяет, является ли символ знаком конца предложения (. ! ?).
// Выход: true, если символ завершает предложение.
bool isSentenceDelimiter(char c) {
    return (c == '.' || c == '!' || c == '?');
}

// Вход: строка текста.
// Что делает: обрезает пробельные символы и переносы по краям строки.
// Выход: очищенная строка.
static std::string trimString(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Вход: сырое слово из текста.
// Что делает: отсекает знаки препинания по краям и приводит русские буквы к нижнему регистру.
// Выход: очищенное русское слово.
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

// Вход: исходный файл, векторы для токенов причастий и структур предложений.
// Что делает: читает файл любого объема, нумерует предложения по базовой схеме и находит причастия.
// Выход: true при успешном разборе; false при ошибке открытия файла.
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
        if (ch == '\n' || ch == '\r') {
            if (!currentSentenceText.empty() && currentSentenceText.back() != ' ') {
                currentSentenceText += ' ';
            }
        }
        else {
            currentSentenceText += ch;
        }

        // Если встречен знак завершения предложения
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

            // Базовая проверка многоточий: фиксируем серию и не увеличиваем счетчик внутри нее
            if (!inDelimiterSequence) {
                inDelimiterSequence = true;
            }
            continue;
        }

        // Выход из серии разделителей на следующий символ
        if (inDelimiterSequence) {
            // Закрывающие скобки и кавычки сразу за знаком оставляем в текущем предложении
            if (ch == ')' || ch == ']' || ch == '"' || ch == '\'' || static_cast<unsigned char>(ch) == 187) {
                continue;
            }

            std::string trimmed = trimString(currentSentenceText);
            // Символ ch уже относится к следующему предложению
            if (!trimmed.empty() && trimmed.back() == ch) {
                trimmed.pop_back();
                trimmed = trimString(trimmed);
            }

            if (!trimmed.empty()) {
                sentences.push_back({ currentSentenceId, trimmed });
                currentSentenceId++;
            }

            currentSentenceText.clear();
            if (ch != '\n' && ch != '\r') {
                currentSentenceText += ch;
            }
            inDelimiterSequence = false;
        }

        // Разделители слов
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);
                if (!clean.empty()) {
                    wordCounter++;
                    if (wordCounter % 1000 == 0) {
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

    // Обработка финального слова и предложения
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

// Вход: путь к файлу и вектор размеченных предложений.
// Что делает: сохраняет предложения построчно в формате нумерованного списка.
// Выход: true при успешной записи; false при ошибке создания файла.
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