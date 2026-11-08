#pragma once
#include <string>
#include <vector>

// Имена внешних конфигурационных файлов словарей
const std::string FILE_SUFFIXES = "participle_suffixes.txt";
const std::string FILE_SHORT = "participle_short.txt";
const std::string FILE_EXCEPTIONS = "participle_exceptions.txt";

// Вход: строка-источник и строка-суффикс.
// Что делает: проверяет, оканчивается ли строка на указанный суффикс/окончание.
// Выход: true, если оканчивается; false в противном случае.
bool endsWith(const std::string& str, const std::string& suffix);

// Вход: строка по ссылке для записи имени отсутствующего файла.
// Что делает: проверяет физическое наличие всех трех внешних файлов словарей на диске.
// Выход: true, если все файлы на месте; false, если хоть один отсутствует.
bool checkDictionaryFiles(std::string& missingFile);

// Вход: нет.
// Что делает: загружает данные из трех внешних файлов словарей в оперативную память.
// Выход: true при успешной загрузке; false при ошибке чтения.
bool loadDictionaries();

// Вход: очищенное слово в нижнем регистре.
// Что делает: проверяет, является ли слово причастием (по короткому словарю или суффиксам).
// Выход: true, если слово определено как причастие; false иначе.
bool isParticiple(const std::string& word);