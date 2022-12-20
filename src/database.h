#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>

class Database {

public:
        Database();
        ~Database();
        bool init(const char *fileName, const long fixedLastWordPointer = 0);
        bool findWord(const char *word);
        char *getWord();
        char *getResult();
        char *getNextRandomWord();
        bool goToNextWord();
        void goToFirstWord();
        void goToLastWord();

private:
        static const int  MAX_WORD_LEN;
        static const int  MAX_DATA_LEN;
        static const char WORD_SEPARATOR;
        static const char DATA_SEPARATOR;

        FILE *dataFile;
        char *dataBuffer;
        char *wordBuffer;
        char *compBuffer;
        long firstWordPointer;
        long lastWordPointer;
        long lastSearchWordPointer;
        long currentWordPointer;

        void toLegalDictionaryWord(const char *word);
        void readData();

};

#endif // DATABASE_H
