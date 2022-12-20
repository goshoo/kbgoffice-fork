#include <string.h>
#include <ctype.h>

#include "database.h"
#include "translator.h"


/*
* This class represents Translator.
* It holds databases and perform some text translation operations.
*
* $Id: translator.cpp 374 2010-05-28 14:01:33Z idimov $
*/


// Declaration about max data and word length
const int Translator::MAX_WORD_LEN = 128;
const int Translator::MAX_WORD_NUM = 8;
const int Translator::MAX_DATA_LEN = 1024 * 256;

// Declaration about dictinonary constants
const int Translator::DUAL  = 1;
const int Translator::EN_BG = 2;
const int Translator::BG_EN = 3;

// Declarations about English Suffixs
const char Translator::EN_SUFFIXES[] = "s,ing,ed";

// Declaration about HTML decoration
const char Translator::BOLD_START[]          = "<B>";
const char Translator::BOLD_END[]            = "</B>";
const char Translator::TRANSCRIPTION_START[] = "<FONT color=\"blue\">";
const char Translator::TRANSCRIPTION_END[]   = "</FONT>";
const char Translator::BREAK_LINE[]          = "<BR>";
const char Translator::A_S_HL_HTML_START[]   = "<FONT color=\"red\">";
const char Translator::A_S_HL_HTML_END[]     = "</FONT>";
const char Translator::A_S_HL_NORMAL_START[] = "<----*";
const char Translator::A_S_HL_NORMAL_END[]   = "*---->";

// Declaration about word delimiter test
const char Translator::DELIMITER = ',';

// Declaration about transcription
const char Translator::TRANSCRIPTION_ANSI[] = "\241\242\243\244\245\246\247\250\251\255";
const char Translator::TRANSCRIPTION_UNICODE[10][8] = {
        "&#0230;", "&#0601;", "&#0596;", "&#0331;", "&#0952;",
        "&#0643;", "&#0652;", "&#0949;", "&#0658;", "&#0240;",
};


Translator::Translator(const int type) {
        dictionaryType            = type;
        separateMeanings          = true;
        boldDecoration            = true;
        htmlOutput                = true;
        tryWithoutSuffix          = true;
        advancedSearchState       = false;
        advancedSearchHighlight   = true;
        advancedSearchWholeWord   = true;
        advancedSearchExactPhrase = false;
        testLevel                 = 0;
        dataBuffer  = new char[MAX_DATA_LEN];
        dataBuffer2 = new char[MAX_DATA_LEN];
        wordBuffer  = new char[MAX_WORD_LEN];
        wordBuffer2 = new char[MAX_WORD_LEN];
        tempBuffer  = new char[MAX_WORD_LEN * MAX_WORD_NUM];
        wordPlus    = new bool[MAX_WORD_NUM];
        dictEnBg = NULL;
        dictBgEn = NULL;
}


Translator::~Translator() {
        delete [] dataBuffer;
        delete [] dataBuffer2;
        delete [] wordBuffer;
        delete [] wordBuffer2;
        delete [] tempBuffer;
        delete [] wordPlus;
        if (dictEnBg != NULL) {
                delete dictEnBg;
        }
        if (dictBgEn != NULL) {
                delete dictBgEn;
        }
}


bool Translator::init(const int type, const char *fileName, const long fixedLastWordPointer) {
        // Check input data
        if ((type != EN_BG) && (type != BG_EN)) {
                return false;
        }
        if ((dictionaryType != DUAL) && (type != dictionaryType)) {
                return false;
        }
        // Init database
        bool ret = false;
        if (type == EN_BG) {
                dictEnBg = new Database();
                ret = dictEnBg->init(fileName, fixedLastWordPointer);
        } else {
                dictBgEn = new Database();
                ret = dictBgEn->init(fileName, fixedLastWordPointer);
        }
        // Set current dictionary
        // If it is dual, en-bg is set as default
        if (dictionaryType == BG_EN) {
                currentDict = dictBgEn;
        } else {
                currentDict = dictEnBg;
        }
        return ret;
}


char *Translator::toLowerCase(const char *word, char *buf) {
        int i = 0;
        while (word[i] != '\0') {
                if (('A' <= word[i]) && (word[i] <= 'Z')) {
                        buf[i] = word[i] - 'A' + 'a';
                } else if (('А' <= word[i]) && (word[i] <= 'Я')) {
                        buf[i] = word[i] - 'А' + 'а';
                } else {
                        buf[i] = word[i];
                }
                i++;
        }
        buf[i] = '\0';
        return buf;
}


char *Translator::toUpperCase(const char *word, char *buf) {
        int i = 0;
        while (word[i] != '\0') {
                if (('a' <= word[i]) && (word[i] <= 'z')) {
                        buf[i] = word[i] + 'A' - 'a';
                } else if (('а' <= word[i]) && (word[i] <= 'я')) {
                        buf[i] = word[i] + 'А' - 'а';
                } else {
                        buf[i] = word[i];
                }
                i++;
        }
        buf[i] = '\0';
        return buf;
}


char *Translator::toLegalDictionaryWord(const char *word, char *buf) {
        int i = 0;
        int j = 0;
        while (word[i] != '\0') {
                if ((('a' <= word[i]) && (word[i] <= 'z')) || (('а' <= word[i]) && (word[i] <= 'я'))) {
                        buf[j++] = word[i];
                } else if (('A' <= word[i]) && (word[i] <= 'Z')) {
                        buf[j++] = word[i] - 'A' + 'a';
                } else if (('А' <= word[i]) && (word[i] <= 'Я')) {
                        buf[j++] = word[i] - 'А' + 'а';
                }
                i++;
        }
        buf[j] = '\0';
        return buf;
}


char *Translator::trimWord(const char *word, char *buf) {
        bool add = false;
        int i = 0;
        int j = 0;
        while (word[i] != '\0') {
                if ((word[i] != ' ') || (add)) {
                        add = true;
                        buf[j++] = word[i];
                }
                i++;
        }
        buf[j] = '\0';
        while ((j > 0) && (buf[j - 1] == ' ')) {
                buf[--j] = '\0';
        }
        return buf;
}


bool Translator::isEnglishWordWithoutSuffix(const char *word, char *buf) {
        char *p;
        char *b;
        strcpy(buf, word);
        strcpy(tempBuffer, EN_SUFFIXES);
        p = tempBuffer - 1;
        while (p != NULL) {
                b = p + 1;
                p = strchr(b, ',');
                if (p != NULL) {
                        *p = '\0';
                }
                if (strlen(buf) > strlen(b)) {
                        char *l = buf + strlen(buf) - strlen(b);
                        if (strcmp(l, b) == 0) {
                                *l = '\0';
                                return true;
                        }
                }
        }
        return false;
}


bool Translator::goToNextWord() {
        return currentDict->goToNextWord();
}


char *Translator::countWords() {
        if (dictionaryType == DUAL) {
                dictEnBg->goToFirstWord();
                int i = 0;
                do {
                        i++;
                } while (dictEnBg->goToNextWord());
                dictBgEn->goToFirstWord();
                int j = 0;
                do {
                        j++;
                } while (dictBgEn->goToNextWord());
                // Put bg first, because the name is dual bg en
                sprintf(tempBuffer, "%d, %d", j, i);
        } else {
                currentDict->goToFirstWord();
                int i = 0;
                do {
                        i++;
                } while (currentDict->goToNextWord());
                sprintf(tempBuffer, "%d", i);
        }
        return tempBuffer;
}


char *Translator::getWord(const bool toUpper) {
        char *ret = NULL;
        ret = currentDict->getWord();
        if (toUpper) {
                ret = toUpperCase(ret, wordBuffer);
        }
        return ret;
}


bool Translator::findWord(const char *word, char **result) {
        bool ret = false;
        toLegalDictionaryWord(word, wordBuffer);
        if (strlen(wordBuffer) == 0) {
                dataBuffer[0] = '\0';
                *result = dataBuffer;
        } else {
                setCurrentDictionary(wordBuffer[0]);
                ret = currentDict->findWord(wordBuffer);
                if ((!ret) && (tryWithoutSuffix) && (isEnglishWordWithoutSuffix(wordBuffer, wordBuffer2)) && (strlen(wordBuffer2) > 1)) {
                        ret = currentDict->findWord(wordBuffer2);
                        if (!ret) {
                                ret = currentDict->findWord(wordBuffer);
                        }
                }
                *result = transformResult(currentDict->getResult());
        }
        return ret;
}


char *Translator::transformResult(const char *result) {
        int j, i, m, n, u;
        char *b, *pu;
        char c;
        strcpy(dataBuffer2, result);
        if ((advancedSearchState) && (advancedSearchHighlight)) {
                for (i = 0; i < wordNumber; i++) {
                        if (wordPlus[i]) {
                                toLowerCase(dataBuffer2, dataBuffer);
                                j = strlen(dataBuffer2);
                                b = dataBuffer;
                                n = 0;
                                while ((b = search(b, tempBuffer + (MAX_WORD_LEN)*i, advancedSearchWholeWord)) != NULL) {
                                        m = b - dataBuffer + 2*n;
                                        memmove(dataBuffer2 + m + 1, dataBuffer2 + m, j - m + 2 + 2*n);
                                        dataBuffer2[m] = '|';
                                        m += strlen(tempBuffer + (MAX_WORD_LEN)*i) + 1;
                                        memmove(dataBuffer2 + m + 1, dataBuffer2 + m, j - m + 2 + 2*n);
                                        dataBuffer2[m] = '|';
                                        b += strlen(tempBuffer + (MAX_WORD_LEN)*i);
                                        n++;
                                }
                        }
                }
        }
        bool currentBold = false;
        bool currentColor = false;
        bool firstLineBreak = true;
        bool transcription = false;
        bool secondLineBreak = false;
        i = 0;
        j = 0;
        while ((c = dataBuffer2[i]) != '\0') {
                if (c == '\n') {
                        if (secondLineBreak) {
                                secondLineBreak = false;
                        }
                        if ((firstLineBreak) && (dataBuffer2[i + 1] != '[')) {
                                firstLineBreak = false;
                                secondLineBreak = true;
                        }
                        if (htmlOutput) {
                                j += appendString(BREAK_LINE, j);
                                if ((separateMeanings) && (!firstLineBreak)) {
                                        j += appendString(BREAK_LINE, j);
                                }
                        } else {
                                dataBuffer[j++] = '\n';
                                if ((separateMeanings) && (!firstLineBreak)) {
                                        dataBuffer[j++] = '\n';
                                }
                        }
                        if (firstLineBreak) {
                                firstLineBreak = false;
                                secondLineBreak = true;
                        }
                } else if ((advancedSearchState) && (advancedSearchHighlight) && (c == '|')) {
                        if (!transcription) {
                                if (htmlOutput) {
                                        // Fix the problem with cross tags
                                        if (currentBold) {
                                                j += appendString(BOLD_END, j);
                                        }
                                        // Append color end or color start tag
                                        j += appendString((currentColor ? A_S_HL_HTML_END : A_S_HL_HTML_START), j);
                                        // Fix the problem with cross tags
                                        if (currentBold) {
                                                j += appendString(BOLD_START, j);
                                        }
                                } else {
                                        j += appendString((currentColor ? A_S_HL_NORMAL_END : A_S_HL_NORMAL_START), j);
                                }
                                currentColor = !currentColor;
                        }
                } else {
                        if ((secondLineBreak) && (c == '[')) {
                                if (htmlOutput) {
                                        j += appendString(TRANSCRIPTION_START, j);
                                }
                                j += appendString("[", j);
                                transcription = true;
                                i++;
                                continue;
                        }
                        if (transcription) {
                                if (c == ']') {
                                        j += appendString("]", j);
                                        if (htmlOutput) {
                                                j += appendString(TRANSCRIPTION_END, j);
                                        }
                                        transcription = false;
                                        i++;
                                        continue;
                                }
                        }
                        if ((htmlOutput) && (boldDecoration) && (!transcription)) {
                                if (currentBold) {
                                        if (!((('A' <= c) && (c <= 'Z')) || (('a' <= c) && (c <= 'z')))) {
                                                j += appendString(BOLD_END, j);
                                                currentBold = false;
                                        }
                                } else {
                                        if ((('A' <= c) && (c <= 'Z')) || (('a' <= c) && (c <= 'z'))) {
                                                j += appendString(BOLD_START, j);
                                                currentBold = true;
                                        }
                                }
                        }
                        pu = (char*)strchr(TRANSCRIPTION_ANSI, c);
                        if ((htmlOutput) && (pu != NULL)) {
                                u = (pu - TRANSCRIPTION_ANSI);
                                j += appendString(TRANSCRIPTION_UNICODE[u], j);
                        } else {
                                dataBuffer[j++] = c;
                        }
                }
                i++;
        }
        if (currentBold) {
                j += appendString(BOLD_END, j);
        }
        if (htmlOutput) {
                j += appendString(BREAK_LINE, j);
        } else {
                dataBuffer[j++] = '\n';
        }
        dataBuffer[j] = '\0';
        return dataBuffer;
}


int Translator::appendString(const char *data, const int currentPointer) {
        int i = 0;
        while (data[i] != '\0') {
                dataBuffer[currentPointer + i] = data[i];
                i++;
        }
        return i;
}


char *Translator::getResult() {
        return transformResult(currentDict->getResult());
}


char *Translator::getNextRandomWord() {
        do {
                currentDict->getNextRandomWord();
        } while ((signed)strlen(currentDict->getResult()) < testLevel);
        return getWord(true);
}


void Translator::setTestParameters(const int level) {
        testLevel = level;
}


bool Translator::testWord(const char *word) {
        char *p;
        char *b;
        int l = 0;
        if (strlen(word) == 0) {
                return false;
        }
        toLowerCase(currentDict->getResult(), dataBuffer);
        // Analize word
        strcpy(tempBuffer, word);
        p = tempBuffer - 1;
        while (p != NULL) {
                b = p + 1;
                p = strchr(b, DELIMITER);
                l = (p == NULL ? strlen(b) : p - b);
                if (l >= MAX_WORD_LEN) {
                        l = MAX_WORD_LEN - 1;
                }
                strncpy(wordBuffer2, b, l);
                wordBuffer2[l] = '\0';
                trimWord(wordBuffer2, wordBuffer2);
                toLowerCase(wordBuffer2, wordBuffer);
                if (wordBuffer[0] != '\0') {
                        toLowerCase(currentDict->getWord(), wordBuffer2);
                        if (strcmp(wordBuffer, wordBuffer2) != 0) {
                                if (search(dataBuffer, wordBuffer, true) != NULL) {
                                        return true;
                                }
                        }
                }
        }
        return false;
}


bool Translator::setAdvancedSearchText(const char *word) {
        int i = 0;
        int j = 0;
        wordNumber = 0;
        bool string = false;
        bool closeWord = false;
        char c;
        char oldc = ' ';
        bool wPlus = true;
        while ((c = word[i]) != '\0') {
                if (c == '"') {
                        string = !string;
                        closeWord = true;
                } else if (string) {
                        if (!((c == ' ') && (oldc == ' '))) {
                                tempBuffer[(MAX_WORD_LEN)*wordNumber + j] = c;
                                j++;
                        }
                } else if ((c == '+') || (c == '-') || ((c == ' ') && (!advancedSearchExactPhrase))) {
                        closeWord = true;
                } else {
                        if (!((c == ' ') && (oldc == ' '))) {
                                tempBuffer[(MAX_WORD_LEN)*wordNumber + j] = c;
                                j++;
                        }
                }
                if (closeWord) {
                        if ((j > 1) || ((j == 1) && (tempBuffer[(MAX_WORD_LEN)*wordNumber] != ' '))) {
                                tempBuffer[(MAX_WORD_LEN)*wordNumber + j] = '\0';
                                wordPlus[wordNumber] = wPlus;
                                wordNumber++;
                                if (wordNumber == MAX_WORD_NUM) {
                                        break;
                                }
                        }
                        if (c == '-') {
                                wPlus = false;
                        } else if (c == '+') {
                                wPlus = true;
                        } else if ((c == ' ') && (j > 0)) {
                                wPlus = true;
                        }
                        j = 0;
                }
                closeWord = false;
                oldc = c;
                i++;
        }
        if (j > 0) {
                if ((j > 1) || ((j == 1) && (tempBuffer[(MAX_WORD_LEN)*wordNumber] != ' '))) {
                        tempBuffer[(MAX_WORD_LEN)*wordNumber + j] = '\0';
                        wordPlus[wordNumber] = wPlus;
                        wordNumber++;
                }
        }
        if (wordNumber > 0) {
                for (i = 0; i < wordNumber; i++) {
                        trimWord(tempBuffer + (MAX_WORD_LEN)*i, tempBuffer + (MAX_WORD_LEN)*i);
                        toLowerCase(tempBuffer + (MAX_WORD_LEN)*i, tempBuffer + (MAX_WORD_LEN)*i);
                }
                advancedSearchState = true;
                setCurrentDictionary(tempBuffer[0]);
                firstTimeAdvancedSearch = true;
                firstDataBaseAdvancedSearch = true;
                return true;
        }
        return false;
}


bool Translator::searchNextWord() {
        int i;
        if (firstTimeAdvancedSearch) {
                currentDict->goToFirstWord();
                firstTimeAdvancedSearch = false;
        }
        else {
                if (!currentDict->goToNextWord()) {
                        return false;
                }
        }
        while (true) {
                do {
                        toLowerCase(currentDict->getResult(), dataBuffer);
                        for (i = 0; i < wordNumber; i++) {
                                if (((wordPlus[i]) && (search(dataBuffer, tempBuffer + (MAX_WORD_LEN)*i, advancedSearchWholeWord) == NULL))
                                        || ((!wordPlus[i]) && (search(dataBuffer, tempBuffer + (MAX_WORD_LEN)*i, advancedSearchWholeWord) != NULL))) {
                                        break;
                                }
                        }
                        if (i == wordNumber) {
                                return true;
                        }
                } while (currentDict->goToNextWord());
                if (firstDataBaseAdvancedSearch) {
                        firstDataBaseAdvancedSearch = false;
                        if (!setCurrentDictionary(tempBuffer[0], true)) {
                                break;
                        }
                        currentDict->goToFirstWord();
                } else {
                        break;
                }
        }
        return false;
}


bool Translator::setCurrentDictionary(const char c, const bool opposite) {
        if (dictionaryType == DUAL) {
                if ((('А' <= c) && (c <= 'Я')) || (('а' <= c) && (c <= 'я'))) {
                        if (opposite) {
                                currentDict = dictEnBg;
                        } else {
                                currentDict = dictBgEn;
                        }
                } else {
                        if (opposite) {
                                currentDict = dictBgEn;
                        } else {
                                currentDict = dictEnBg;
                        }
                }
                return true;
        }
        return false;
}


char *Translator::search(char *text, const char *word, const bool exactSearch) {
        if (exactSearch) {
                char *b = text - 1;
                while (b != NULL) {
                        b++;
                        b = strstr(b, word);
                        if ( (b != NULL) &&
                             ( (b == text) || ( (b > text) && (!isAlphaChar(text[b - text - 1]))) ) &&
                             ( ( ( b - text + strlen(word) < strlen(text) ) &&
                                 ( !isAlphaChar(text[b - text + strlen(word)]) ) ) ||
                               ( b - text + strlen(word) == strlen(text) )
                             )
                           ) {
                                return b;
                        }
                }
        } else {
                return strstr(text, word);
        }
        return NULL;
}


bool Translator::isAlphaChar(const char c) {
        return (
                (('A' <= c) && (c <= 'Z')) ||
                (('a' <= c) && (c <= 'z')) ||
                (('А' <= c) && (c <= 'Я')) ||
                (('а' <= c) && (c <= 'я'))
        );
}


char *Translator::extractText(const char *text) {
        int j = 0;
        int i = 0;
        bool tag = false;
        char c;
        while (((c = text[i]) != '\0') && (j < MAX_DATA_LEN)) {
                if (c == '<') {
                        tag = true;
                        if (strncmp(text + i, BREAK_LINE, strlen(BREAK_LINE)) == 0) {
                                dataBuffer[j++] = '\n';
                        }
                } else if (c == '>') {
                        tag = false;
                } else if (!tag) {
                        dataBuffer[j++] = c;
                }
                i++;
        }
        dataBuffer[j] = '\0';
        return dataBuffer;
}


char *Translator::getAbbreviations(const char *abbrFileName) {
        FILE *f;
        f = fopen(abbrFileName, "r");
        dataBuffer[0] = '\0';
        dataBuffer2[0] = '\0';
        if (f != NULL) {
                while (fgets(tempBuffer, MAX_WORD_LEN, f) != NULL) {
                        strcat(dataBuffer2, tempBuffer);
                }
                fclose(f);

                char c;
                int i = 0;
                int j = 0;
                if (htmlOutput) {
                        j += appendString("<H2>", j);
                } else {
                        dataBuffer[j++] = '\n';
                }
                j += appendString("Съкращения/Abbreviations", j);
                if (htmlOutput) {
                        j += appendString("</H2><TABLE><TR><TD>", j);
                } else {
                        dataBuffer[j++] = '\n';
                        dataBuffer[j++] = '\n';
                        dataBuffer[j++] = '\n';
                }
                if (htmlOutput) {
                        while ((c = dataBuffer2[i]) != '\0') {
                                if (c == '\n') {
                                        j += appendString("</TD></TR><TR><TD>", j);
                                } else if (c == '\t') {
                                        j += appendString("</TD><TD>", j);
                                        if (dataBuffer2[i + 1] == '\t') {
                                                i++;
                                        }
                                } else {
                                        dataBuffer[j++] = c;
                                }
                                i++;
                        }
                        j += appendString("</TD></TR></TABLE>", j);
                } else {
                        while ((c = dataBuffer2[i]) != '\0') {
                                dataBuffer[j++] = c;
                                i++;
                        }
                        dataBuffer[j++] = '\n';
                        dataBuffer[j++] = '\n';
                }
                dataBuffer[j] = '\0';
        }
        return dataBuffer;
}
