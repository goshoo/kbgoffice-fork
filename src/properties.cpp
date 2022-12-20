#include <stdio.h>
#include <string.h>

#include "properties.h"


/*
* This class represents Properties.
* Search data in Configuration file, read and save key values.
*
* $Id: properties.cpp 369 2010-04-30 12:37:42Z idimov $
*/


// Define max line and key length
const int Properties::MAX_LINE_LEN  = 1024;
const int Properties::MAX_KEY_LEN   = 128;


Properties::Properties(const char *path, const char *file, const char *suffix) {
        // Allocate memory
        buf = new char[MAX_LINE_LEN];
        sbuf = new char[MAX_LINE_LEN];
        key = new char[MAX_KEY_LEN];
        fileName = new char[strlen(path) + strlen(file) + 2];
        backupFileName = new char[strlen(path) + strlen(file) + strlen(suffix) + 2];
        // Copy file names in local variables
        strcpy(fileName, path);
        if ((path[0] != '\0') && (path[strlen(path) - 1] != '/')) {
                strcat(fileName, "/");
        }
        strcat(fileName, file);
        strcpy(backupFileName, fileName);
        strcat(backupFileName, suffix);
}


Properties::~Properties() {
        delete [] buf;
        delete [] sbuf;
        delete [] key;
        delete [] fileName;
        delete [] backupFileName;
}


char *Properties::getString(const char *property, const char *defaultValue) {
        bool found = false;
        char *ret = NULL;
        FILE *f;
        f = fopen(fileName, "r");
        if (f != NULL) {
                strcpy(key, property);
                if (key[strlen(key) - 1] != '=') {
                        strcat(key, "=");
                }
                while (fgets(buf, MAX_LINE_LEN, f) != NULL) {
                        if (strstr(buf, key) == buf) {
                                ret = buf + strlen(key);
                                if ((ret[0] != '\0') && (ret[strlen(ret) - 1] == '\n')) {
                                        ret[strlen(ret) - 1] = '\0';
                                }
                                found = true;
                                break;
                        }
                }
                fclose(f);
        }
        if (!found) {
                strcpy(buf, defaultValue);
                ret = buf;
        }
        return ret;
}


int Properties::getInt(const char *property, const int defaultValue) {
        int ret = defaultValue;
        char *p;
        p = getString(property);
        if (strlen(p) > 0) {
                sscanf(p, "%d", &ret);
        }
        return ret;
}


int Properties::getInt(const char *property, const int defaultValue, const int minValue, const int maxValue) {
        int ret = getInt(property, defaultValue);
        ret = (ret < minValue ? minValue : ret);
        ret = (ret > maxValue ? maxValue : ret);
        return ret;
}


bool Properties::getBool(const char *property, const bool defaultValue) {
        bool ret = defaultValue;
        char *p;
        p = getString(property);
        if (strlen(p) > 0) {
                ret = (strcmp(p, "true") == 0);
        }
        return ret;
}


bool Properties::setString(const char *property, const char *value) {
        bool ret = true;
        FILE *f;
        FILE *t;
        strcpy(key, property);
        if (key[strlen(key) - 1] != '=') {
                strcat(key, "=");
        }
        strcpy(sbuf, key);
        strcat(sbuf, value);
        strcat(sbuf, "\n");
        f = fopen(fileName, "r");
        if (f == NULL) {
                f = fopen(fileName, "w");
                if (f != NULL) {
                        fputs(sbuf, f);
                        fclose(f);
                } else {
                        ret = false;
                }
                return ret;
        }
        t = fopen(backupFileName, "w");
        bool found = false;
        if ((f != NULL) && (t != NULL)) {
                while (fgets(buf, MAX_LINE_LEN, f) != NULL) {
                        if (strstr(buf, key) == buf) {
                                fputs(sbuf, t);
                                found = true;
                        } else {
                                fputs(buf, t);
                        }
                }
                if (!found) {
                        if ((buf[0] != '\0') && (buf[strlen(buf) - 1] != '\n')) {
                                fputs("\n", t);
                        }
                        fputs(sbuf, t);
                }
                fclose(f);
                fclose(t);
                remove(fileName);
                rename(backupFileName, fileName);
        } else {
                ret = false;
        }
        return ret;
}


bool Properties::setInt(const char *property, const int value) {
        char c[16];
        sprintf(c, "%d", value);
        return setString(property, c);
}


bool Properties::setBool(const char *property, const bool value) {
        return setString(property, (value ? "true" : "false"));
}
