// TextParser.cpp
#include "TextParser.h"
#include "ParticipleDetector.h"
#include <iostream>
#include <fstream>

// Вход: символ.
// Что делает: проверяет принадлежность к кириллице Windows-1251 (192..255, 168 - Ё, 184 - ё).
// Выход: true, если символ русский.
bool isRussianLetter(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return (uc >= 192 && uc <= 255) || (uc == 168) || (uc == 184);
}

// Вход: символ в кодировке Windows-1251.
// Что делает: преобразует заглавные русские буквы (А-Я, Ё) в строчные (а-я, ё).
// Выход: строчный русский символ.
char toLowerRus(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    // Русские буквы А-Я -> а-я
    if (uc >= 192 && uc <= 223) {
        return static_cast<char>(uc + 32);
    }
    // Русская буква Ё -> ё
    if (uc == 168) {
        return static_cast<char>(184);
    }
    return c;
}

// Вход: символ текста.
// Что делает: проверяет знаки завершения предложения ('.', '!', '?').
// Выход: true, если символ завершает предложение.
bool isSentenceDelimiter(char c) {
    return (c == '.' || c == '!' || c == '?');
}

// Вход: сырое слово из текста.
// Что делает: отсекает небуквенные символы по краям и приводит русские буквы к нижнему регистру.
// Выход: очищенное русское слово.
std::string sanitizeWord(const std::string& raw) {
    if (raw.empty()) {
        return "";
    }

    // Ищем первую русскую букву с начала
    size_t start = 0;
    while (start < raw.length() && !isRussianLetter(raw[start])) {
        start++;
    }

    // Если в токене вообще нет русских букв (например, число "123" или смайлик)
    if (start >= raw.length()) {
        return "";
    }

    // Ищем последнюю русскую букву с конца
    size_t end = raw.length() - 1;
    while (end > start && !isRussianLetter(raw[end])) {
        end--;
    }

    // Собираем очищенное слово в нижнем регистре (внутренние дефисы сохраняются)
    std::string result = "";
    for (size_t i = start; i <= end; ++i) {
        result += toLowerRus(raw[i]);
    }

    return result;
}

// Вход: имя файла (filename) и булева переменная по ссылке для возврата статуса ошибки (isOk).
// Что делает: выполняет потоковый лексический анализ текста:
//             1. Посимвольно считывает файл любого размера через буфер потока без переполнения ОЗУ.
//             2. Ведет сквозной подсчет номеров предложений с подавлением серий знаков (например, многоточий "...").
//             3. Выделяет слова, очищает их от пунктуации и приводит к нижнему регистру.
//             4. Выводит индикацию работы (точка каждые 200 слов), чтобы показать активность программы.
//             5. Фильтрует причастия через ParticipleDetector и сохраняет их вместе с номером предложения.
// Выход: вектор структур WordToken (каждая содержит слово-причастие и номер его предложения).
std::vector<WordToken> extractParticiplesFromFile(const std::string& filename, bool& isOk) {
    // Результирующий динамический массив для хранения отобранных причастий
    std::vector<WordToken> participles;

    // Открываем файловый поток для чтения
    // Имя передается через c_str() для совместимости со стандартными потоками
    std::ifstream file(filename.c_str());

    // Проверяем физическую доступность файла
    if (!file.is_open()) {
        isOk = false;       
        return participles;
    }

    isOk = true; // Файл успешно открыт, начинаем разбор


    // Номер текущего предложения (нумерация с 1)
    int currentSentence = 1;
    // Счетчик всех распознанных слов в тексте (нужен для индикации прогресса)
    long long wordCounter = 0;
    // Буфер накопления текущего считываемого слова (пока не встретится пробел или знак препинания)
    std::string currentToken = "";

    // Символ для посимвольного чтения из потока
    char ch = 0;

    // Флаг защиты от многоточий и серий знаков ("...", "?!", "!!")
    // true, если предыдущий символ был знаком конца предложения, false — если обычный символ
    bool inDelimiterSequence = false;

    // ОСНОВНОЙ ЦИКЛ: Читаем файл посимвольно до конца потока.
    while (file.get(ch)) {

        // Если встречен знак конца предложения ('.', '!', '?')
        if (isSentenceDelimiter(ch)) {

            // Если перед точкой/знаком в буфере копилось слово — его нужно завершить
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);

                // Если после очистки от мусора осталось корректное слово
                if (!clean.empty()) {
                    wordCounter++;

                    // Каждые 200 слов печатаем точку в консоль чтобы показать что программа не зависла
                    if (wordCounter % 200 == 0) {
                        std::cout << ".";
                    }

                    // Проверяем, является ли слово причастием
                    if (isParticiple(clean)) {
                        participles.push_back({ clean, currentSentence });
                    }
                }
                // Очищаем буфер слова для приема следующего токена
                currentToken.clear();
            }

            // ЛОГИКА МНОГОТОЧИЙ:
            // Увеличиваем номер предложения только на первом знаке из серии (например, на первой точке из трех).
            // Вторая и третья точки условия не пройдут, так как inDelimiterSequence уже равен true.
            if (!inDelimiterSequence) {
                currentSentence++;
                inDelimiterSequence = true; // Фиксируем, что мы внутри серии знаков
            }

            // Переходим к следующему символу файла
            continue;
        }

        // Если символ НЕ является знаком конца предложения — сбрасываем флаг серии знаков
        inDelimiterSequence = false;

        // Если встречен пробельный разделитель (пробел, табуляция, перенос строки)
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {

            // Если перед пробелом было накоплено слово — обрабатываем его
            if (!currentToken.empty()) {
                std::string clean = sanitizeWord(currentToken);

                if (!clean.empty()) {
                    wordCounter++;

                    // Каждые 200 слов печатаем точку в консоль чтобы показать что программа не зависла
                    if (wordCounter % 200 == 0) {
                        std::cout << ".";
                    }

                    // Проверяем, является ли слово причастием
                    if (isParticiple(clean)) {
                        participles.push_back({ clean, currentSentence });
                    }
                }
                // Очищаем буфер под следующее слово
                currentToken.clear();
            }
        }
        // Если встречен обычный буквенный или внутренний символ слова (буква, внутренний дефис)
        else {
            // Накапливаем символ в буфер текущего слова
            currentToken += ch;
        }
    }

    // ПОСТ-ОБРАБОТКА ПОСЛЕДНЕГО СЛОВА:
    // Если файл завершился словом без завершающего пробела или точки в самом конце,
    // в currentToken останется слово, которое также необходимо обработать.
    if (!currentToken.empty()) {
        std::string clean = sanitizeWord(currentToken);
        if (!clean.empty() && isParticiple(clean)) {
            participles.push_back({ clean, currentSentence });
        }
    }

    // Закрываем файловый дескриптор/поток
    file.close();

    // Возвращаем итоговый список отобранных структур
    return participles;
}