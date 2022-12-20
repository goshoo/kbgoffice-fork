#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "database.h"


/*
* This class represents Database. It search and read data from text file
*
* Database file must be in plain text with delimiters between different
* data '\0'. It is useful because C/C++ uses '\0' to terminate a strings.
* The structure must be following: Word must be first. It must be
* followed by delimiter '\n'. It also can be used to divide different
* meanings in translation data. After '\n' starts a translation data
* (result). And it ends of course with '\0'. And after that starts new
* word. Data file must be sorted by contents of words. That means
* excluding spaces, dashes and other non alpha chars and sorting data
* without them. Also database cannot include two equal contents of words.
* That means "SECOND-HAND" and "SECOND HAND" are the same words and it is
* "SECONDHAND". Also database must starts and ends with '\0'. In this way
* search method does not need to make check of beginning and end of file.
*
* Method uses binary search to find a specified word. The problem is that
* database is not with fixed records (positions of words) and each time
* it goes somewhere in the middle in the translation data. In this case
* method read forward data to meet data delimiter ('\0'). If it meets
* end of binary search position then it look backward to ensure that
* no more words in search interval. When it meet data delimiter it
* compare extracted word with entered word and continue to search
* depending of result or stop execution if founds word. If method does
* not found a word it returns a nearest similar word in search interval.
*
* The forward search is increased by using dataBuffer, but backward
* search read every byte separately. It is not big problem because method
* uses only forward search in most cases.
*/


// Constant declaration
const int  Database::MAX_WORD_LEN      = 128;
const int  Database::MAX_DATA_LEN      = 1024 * 32;
const char Database::WORD_SEPARATOR    = '\n';
const char Database::DATA_SEPARATOR    = '\0';


Database::Database():
    firstWordPointer(0),
    lastWordPointer(0),
    lastSearchWordPointer(0),
    currentWordPointer(0)
{
        dataBuffer = new char[MAX_DATA_LEN];
        wordBuffer = new char[MAX_WORD_LEN];
        compBuffer = new char[MAX_WORD_LEN];
        dataFile = NULL;
}


Database::~Database() {
        delete [] dataBuffer;
        delete [] wordBuffer;
        delete [] compBuffer;
        if (dataFile != NULL) {
                fclose(dataFile);
        }
}


bool Database::init(const char *fileName, const long fixedLastWordPointer) {
        // Ensure against invoking twice
        if (dataFile != NULL) {
                return false;
        }
        // Set buffers to zero length
        dataBuffer[0] = '\0';
        wordBuffer[0] = '\0';
        compBuffer[0] = '\0';
        // Open DataFile
        dataFile = fopen(fileName, "r");
        // Return false if failed
        if (dataFile == NULL) {
                return false;
        }
        // Set First Word Pointer
        firstWordPointer = 0;
        // Set Last Word Pointer
        if (fixedLastWordPointer > 0) {
                lastWordPointer = fixedLastWordPointer;
        }
        else {
                fseek(dataFile, -2L, SEEK_END);
                lastWordPointer = ftell(dataFile);
                do {
                        lastWordPointer--;
                        fseek(dataFile, lastWordPointer, SEEK_SET);
                        size_t read = fread(dataBuffer, 1, 1, dataFile);
                        (void)read;
                } while (dataBuffer[0] != DATA_SEPARATOR);
        }
        // Set Current and Next Word Pointer
        currentWordPointer = firstWordPointer;
        lastSearchWordPointer = firstWordPointer;
        // Read (load) data in buffers
        readData();
        // Set data in compBuff
        toLegalDictionaryWord(dataBuffer);
        return true;
}


void Database::goToFirstWord() {
        currentWordPointer = firstWordPointer;
        readData();
}


void Database::goToLastWord() {
        currentWordPointer = lastWordPointer;
        readData();
}


bool Database::findWord(const char *word) {
        long b, e;
        long m, rm;
        int comp;
        int pos;

        // Calculating the beginning and the end of search area
        b = firstWordPointer;
        e = lastWordPointer;
        comp = strcmp(compBuffer, word);
        if (comp < 0) {
                b = lastSearchWordPointer;
        } else if (comp > 0) {
                e = lastSearchWordPointer;
        } else {
                currentWordPointer = lastSearchWordPointer;
                readData();
                return true;
        }

        // Search for word
        while (true) {
                m = (b + e) / 2;
                rm = m;
                fseek(dataFile, m, SEEK_SET);
                size_t read = fread(dataBuffer, 1, MAX_DATA_LEN, dataFile);
                pos = strlen(dataBuffer);
                m += pos;
                toLegalDictionaryWord(dataBuffer + pos);
                if (m == e) {
                        comp = strcmp(compBuffer, word);
                        if (comp <= 0) {
                                break;
                        } else {
                                m = rm;
                                fseek(dataFile, m, SEEK_SET);
                                read = fread(dataBuffer, 1, 1, dataFile);
                                while (dataBuffer[0] != DATA_SEPARATOR) {
                                        m--;
                                        fseek(dataFile, m, SEEK_SET);
                                        read = fread(dataBuffer, 1, 1, dataFile);
                                }
                                read = fread(dataBuffer, 1, MAX_WORD_LEN, dataFile);
                                toLegalDictionaryWord(dataBuffer);
                                if (m == b) {
                                        comp = strcmp(compBuffer, word);
                                        if (comp < 0) {
                                                m = e;
                                        }
                                        break;
                                }
                        }
                }
                comp = strcmp(compBuffer, word);
                if (comp < 0) {
                        b = m;
                } else if (comp > 0) {
                        e = m;
                } else {
                        break;
                }
        }
        lastSearchWordPointer = m;
        currentWordPointer = m;
        readData();
        toLegalDictionaryWord(dataBuffer);
        return (comp == 0);
}


void Database::toLegalDictionaryWord(const char *word) {
        int i = 0, j = 0;
        while (word[i] != WORD_SEPARATOR) {
                if ((('a' <= word[i]) && (word[i] <= 'z')) || (('à' <= word[i]) && (word[i] <= 'ÿ'))) {
                        compBuffer[j++] = word[i];
                } else if (('A' <= word[i]) && (word[i] <= 'Z')) {
                        compBuffer[j++] = word[i] - 'A' + 'a';
                } else if (('À' <= word[i]) && (word[i] <= 'ß')) {
                        compBuffer[j++] = word[i] - 'À' + 'à';
                }
                i++;
        }
        compBuffer[j] = '\0';
}


void Database::readData() {
        fseek(dataFile, currentWordPointer + 1, SEEK_SET);
        size_t read = fread(dataBuffer, 1, MAX_DATA_LEN, dataFile);
        (void)read;
        int i = 0;
        while (dataBuffer[i] != WORD_SEPARATOR) {
                wordBuffer[i] = dataBuffer[i];
                i++;
        }
        wordBuffer[i] = '\0';
}


char *Database::getWord() {
        return wordBuffer;
}


char *Database::getResult() {
        return dataBuffer;
}


bool Database::goToNextWord() {
        bool ret = false;
        if (currentWordPointer < lastWordPointer) {
                ret = true;
                currentWordPointer += (strlen(dataBuffer) + 1);
        }
        readData();
        return ret;
}


char *Database::getNextRandomWord() {
        long pos = firstWordPointer + (long)((((double)lastWordPointer) * rand()) / (RAND_MAX + (double)firstWordPointer));
        if (pos < firstWordPointer) {
                pos = firstWordPointer;
        }
        if (pos > lastWordPointer) {
                pos = lastWordPointer;
        }
        fseek(dataFile, pos, SEEK_SET);
        size_t read = fread(dataBuffer, 1, MAX_DATA_LEN, dataFile);
        (void)read;
        currentWordPointer = pos + strlen(dataBuffer);
        readData();
        return wordBuffer;
}
