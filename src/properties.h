#ifndef PROPERTIES_H
#define PROPERTIES_H

class Properties {

public:
        Properties(const char *path, const char *file, const char *suffix = "~");
        ~Properties();
        char *getString(const char *property, const char *defaultValue = "");
        int getInt(const char *property, const int defaultValue = 0);
        int getInt(const char *property, const int defaultValue, const int minValue, const int maxValue);
        bool getBool(const char *property, const bool defaultValue = false);
        bool setString(const char *property, const char *value);
        bool setInt(const char *property, const int value);
        bool setBool(const char *property, const bool value);

private:
        static const int MAX_LINE_LEN;
        static const int MAX_KEY_LEN;
        char *buf;
        char *sbuf;
        char *key;
        char *fileName;
        char *backupFileName;

};

#endif // PROPERTIES_H
