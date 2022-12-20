#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

#include "properties.h"
#include "translator_manager.h"


/*
* This class holds translators.
* It reads data from configuration files, initialize and manages dictionaries.
*
* Current solution is not good. The best way is Translator object to keep
* references to different databases. But in this case there is problem with
* functionality. Currently Translator trace difference between cyrillic and
* latin script and suggest which dictionary to use if there are Dual dictionary
* (e.g. two).
*
*/

TranslatorManager::TranslatorManager(char *pDataDir) {
        dataDir = pDataDir;
}


TranslatorManager::~TranslatorManager() {
        for (unsigned int i = 0; i < translators.size(); i++) {
                delete translators[i];
        }
}


bool TranslatorManager::init(const bool ignoreLANG) {
        // Determine LANG env
        char *lang = getenv("LANG");
        bool isBG = (!ignoreLANG) && (lang != NULL) && (lang[0] == 'b') && (lang[1] == 'g');
        // Load Dictionaries
        vector<string> dict = findFiles(dataDir, ".dict");
        if (dict.size() == 0) {
                cerr << "TranslatorManager::init - There is no dictionary files in directory: " << dataDir << "\n";
                return false;
        }
        for (unsigned int i = 0; i < dict.size(); i++) {
                Properties *prop = new Properties(dataDir, dict[i].c_str());
                struct Dictionary d;
                d.file = dict[i];
                d.version = string(prop->getString("version"));
                d.icon = string(dataDir) + prop->getString("icon");
                d.name = string(prop->getString(isBG ? "name.bg" : "name"));
                dictionaries.push_back(d);
                string type = string(prop->getString("type"));
                Translator *tr;
                bool isDataOk = true;
                if (type.compare("dual") == 0) {
                        tr = new Translator(tr->DUAL);
                        isDataOk = isDataOk && tr->init(tr->EN_BG, string(string(dataDir) + prop->getString("data.en")).c_str());
                        isDataOk = isDataOk && tr->init(tr->BG_EN, string(string(dataDir) + prop->getString("data.bg")).c_str());
                } else if (type.compare("en") == 0) {
                        tr = new Translator(tr->EN_BG);
                        isDataOk = isDataOk && tr->init(tr->EN_BG, string(string(dataDir) + prop->getString("data")).c_str());
                } else if (type.compare("bg") == 0) {
                        tr = new Translator(tr->BG_EN);
                        isDataOk = isDataOk && tr->init(tr->BG_EN, string(string(dataDir) + prop->getString("data")).c_str());
                } else {
                        cerr << "TranslatorManager::init - Wrong description file: " << dict[i] << "\n";
                        return false;
                }
                if (!isDataOk) {
                        cerr << "TranslatorManager::init - Problem initializing dictionary: " << dict[i] << "\n";
                        return false;
                }
                translators.push_back(tr);
                delete prop;
        }
        currentDict = 0;
        currentTranslator = translators[0];

        // Load TestDictionaries
        vector<string> test = findFiles(dataDir, ".test");
        for (unsigned int i = 0; i < test.size(); i++) {
                Properties *prop = new Properties(dataDir, test[i].c_str());
                struct TestDictionary td;
                td.file = test[i];
                td.name = string(prop->getString(isBG ? "name.bg" : "name"));
                testDictionaries.push_back(td);
                delete prop;
        }

        return true;
}


int TranslatorManager::sizeOfDictionaries() {
        return dictionaries.size();
}


Dictionary TranslatorManager::getDictionary(const int index) {
        return dictionaries[index];
}


void TranslatorManager::setCurrentDictionary(const int index) {
        if ((0 <= index) && ((unsigned)index < translators.size())) {
                currentTranslator = translators[index];
                currentDict = index;
        }
}


int TranslatorManager::getCurrentDictionary() {
        return currentDict;
}


int TranslatorManager::sizeOfTestDictionaries() {
        return testDictionaries.size();
}


TestDictionary TranslatorManager::getTestDictionary(const int index) {
        return testDictionaries[index];
}


Translator *TranslatorManager::getTestDictionaryObject(const int index, const int level) {
        Properties *prop = new Properties(dataDir, testDictionaries[index].file.c_str());
        string type = string(prop->getString("type"));
        Translator *tr;
        bool isDataOk = true;
        if (type.compare("en") == 0) {
                tr = new Translator(tr->EN_BG);
                isDataOk = isDataOk && tr->init(tr->EN_BG, string(string(dataDir) + prop->getString("data")).c_str());
        } else if (type.compare("bg") == 0) {
                tr = new Translator(tr->BG_EN);
                isDataOk = isDataOk && tr->init(tr->BG_EN, string(string(dataDir) + prop->getString("data")).c_str());
        } else {
                cerr << "TranslatorManager::getTestDictionaryObject - Wrong description file: " << testDictionaries[index].file << "\n";
                return false;
        }
        if (!isDataOk) {
                cerr << "TranslatorManager::getTestDictionaryObject - Problem initialazing dictionary: " << testDictionaries[index].file << "\n";
                return false;
        }
        char c[16];
        sprintf(c, "%d", level);
        string s = string("level_") + string(c);
        tr->setTestParameters(prop->getInt(s.c_str()));
        delete prop;
        return tr;
}


bool TranslatorManager::findWord(const char *word, char **result) {
        return currentTranslator->findWord(word, result);
}


bool TranslatorManager::goToNextWord() {
        return currentTranslator->goToNextWord();
}


char *TranslatorManager::getWord(const bool toUpper) {
        return currentTranslator->getWord(toUpper);
}


char *TranslatorManager::getResult() {
        return currentTranslator->getResult();
}


bool TranslatorManager::setAdvancedSearchText(const char *word) {
        return currentTranslator->setAdvancedSearchText(word);
}


bool TranslatorManager::searchNextWord() {
        return currentTranslator->searchNextWord();
}


char *TranslatorManager::countWords() {
        return currentTranslator->countWords();
}


char *TranslatorManager::getAbbreviations() {
        return currentTranslator->getAbbreviations(string(string(dataDir) + "abbr.dat").c_str());
}


bool TranslatorManager::getSeparateMeanings() {
        return currentTranslator->separateMeanings;
}


bool TranslatorManager::getTryWithoutSuffix() {
        return currentTranslator->tryWithoutSuffix;
}


bool TranslatorManager::getBoldDecoration() {
        return currentTranslator->boldDecoration;
}


bool TranslatorManager::getHTMLOutput() {
        return currentTranslator->htmlOutput;
}


bool TranslatorManager::getAdvancedSearchWholeWord() {
        return currentTranslator->advancedSearchWholeWord;
}


bool TranslatorManager::getAdvancedSearchExactPhrase() {
        return currentTranslator->advancedSearchExactPhrase;
}


bool TranslatorManager::getAdvancedSearchHighlight() {
        return currentTranslator->advancedSearchHighlight;
}


bool TranslatorManager::getAdvancedSearchState() {
        return currentTranslator->advancedSearchState;
}


void TranslatorManager::setSeparateMeanings(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->separateMeanings = val;
        }
}


void TranslatorManager::setTryWithoutSuffix(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->tryWithoutSuffix = val;
        }
}


void TranslatorManager::setBoldDecoration(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->boldDecoration = val;
        }
}


void TranslatorManager::setHTMLOutput(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->htmlOutput = val;
        }
}


void TranslatorManager::setAdvancedSearchWholeWord(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->advancedSearchWholeWord = val;
        }
}


void TranslatorManager::setAdvancedSearchExactPhrase(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->advancedSearchExactPhrase = val;
        }
}


void TranslatorManager::setAdvancedSearchHighlight(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->advancedSearchHighlight = val;
        }
}


void TranslatorManager::setAdvancedSearchState(const bool val) {
        for (unsigned int i = 0; i < translators.size(); i++) {
                translators[i]->advancedSearchState = val;
        }
}


vector<string> TranslatorManager::findFiles(const char *dir, const char *extension) {
        int l = strlen(extension);
        vector<string> result;
        DIR *d;
        struct dirent *e;

        if ((d = opendir(dir)) == NULL) {
                cerr << "TranslatorManager::findFiles - Unable to open directory: " << dir << "\n";
                return result;
        }

        while ((e = readdir(d)) != NULL) {
                string fn(e->d_name);
                size_t p = fn.rfind(extension);
                if ((p != string::npos)  && (p == fn.size() - l)) {
                        result.push_back(fn);
                }
        }

        if (closedir(d) != 0) {
            cerr << "TranslatorManager::findFiles - Unable to close directory: " << dir << "\n";
        }

        sort(result.begin(), result.end());
        return result;
}
