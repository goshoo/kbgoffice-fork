#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "database.h"

class Translator {

public:
        Translator(const int type);
        ~Translator();
        bool init(const int type, const char *fileName, const long fixedLastWordPointer = 0);
        bool findWord(const char *word, char **result);
        bool goToNextWord();
        char *getWord(const bool toUpper = false);
        char *getResult();
        bool testWord(const char *word);
        char *getNextRandomWord();
        void setTestParameters(const int level);
        bool setAdvancedSearchText(const char *word);
        bool searchNextWord();
        char *extractText(const char *text);
        char *countWords();
        char *getAbbreviations(const char *abbrFileName);

        static const int DUAL;
        static const int EN_BG;
        static const int BG_EN;

        bool separateMeanings;
        bool boldDecoration;
        bool htmlOutput;

        bool tryWithoutSuffix;

        bool advancedSearchState;
        bool advancedSearchHighlight;
        bool advancedSearchWholeWord;
        bool advancedSearchExactPhrase;

private:
        static const int  MAX_DATA_LEN;
        static const int  MAX_WORD_LEN;
        static const int  MAX_WORD_NUM;

        static const char EN_SUFFIXES[];
        static const char BOLD_START[];
        static const char BOLD_END[];
        static const char TRANSCRIPTION_START[];
        static const char TRANSCRIPTION_END[];
        static const char A_S_HL_HTML_START[];
        static const char A_S_HL_HTML_END[];
        static const char A_S_HL_NORMAL_START[];
        static const char A_S_HL_NORMAL_END[];
        static const char BREAK_LINE[];
        static const char DELIMITER;
        static const char TRANSCRIPTION_UNICODE[10][8];
        static const char TRANSCRIPTION_ANSI[];

        int dictionaryType;

        Database *dictEnBg;
        Database *dictBgEn;
        Database *currentDict;

        char *dataBuffer;
        char *wordBuffer;
        char *tempBuffer;
        char *dataBuffer2;
        char *wordBuffer2;
        bool *wordPlus;

        int wordNumber;

        int testLevel;

        bool firstTimeAdvancedSearch;
        bool firstDataBaseAdvancedSearch;

        char *toLowerCase(const char *word, char *buf);
        char *toUpperCase(const char *word, char *buf);
        char *toLegalDictionaryWord(const char *word, char *buf);
        char *trimWord(const char *word, char *buf);
        bool isEnglishWordWithoutSuffix(const char *word, char *buf);
        char *transformResult(const char *result);
        int appendString(const char *data, const int currentPointer);
        bool setCurrentDictionary(const char c, const bool opposite = false);
        char *search(char *text, const char *word, const bool exactSearch);
        bool isAlphaChar(const char c);

};

#endif // TRANSLATOR_H
