#ifndef TRANSLATOR_MANAGER_H
#define TRANSLATOR_MANAGER_H

#include <string>
#include <vector>

using namespace std;

#include "translator.h"

struct Dictionary {
        string file;
        string name;
        string icon;
        string version;
};

struct TestDictionary {
        string file;
        string name;
};


class TranslatorManager {

public:
        TranslatorManager(char *pDataDir);
        ~TranslatorManager();
        bool init(const bool ignoreLANG = false);

        int sizeOfDictionaries();
        Dictionary getDictionary(const int index);
        void setCurrentDictionary(const int index);
        int getCurrentDictionary();

        int sizeOfTestDictionaries();
        TestDictionary getTestDictionary(const int index);
        Translator *getTestDictionaryObject(const int index, const int level);

        bool findWord(const char *word, char **result);
        bool goToNextWord();
        char *getWord(const bool toUpper = false);
        char *getResult();
        bool setAdvancedSearchText(const char *word);
        bool searchNextWord();
        char *countWords();
        char *getAbbreviations();

        bool getSeparateMeanings();
        bool getTryWithoutSuffix();
        bool getBoldDecoration();
        bool getHTMLOutput();
        bool getAdvancedSearchWholeWord();
        bool getAdvancedSearchExactPhrase();
        bool getAdvancedSearchHighlight();
        bool getAdvancedSearchState();

        void setSeparateMeanings(const bool val);
        void setTryWithoutSuffix(const bool val);
        void setBoldDecoration(const bool val);
        void setHTMLOutput(const bool val);
        void setAdvancedSearchWholeWord(const bool val);
        void setAdvancedSearchExactPhrase(const bool val);
        void setAdvancedSearchHighlight(const bool val);
        void setAdvancedSearchState(const bool val);


private:
        char *dataDir;

        vector<Dictionary> dictionaries;
        vector<TestDictionary> testDictionaries;
        vector<Translator*> translators;

        Translator *currentTranslator;
        int currentDict;

        vector<string> findFiles(const char *dir, const char *extension);

};

#endif // TRANSLATOR_MANAGER_H
