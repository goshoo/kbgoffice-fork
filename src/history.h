#ifndef HISTORY_H
#define HISTORY_H

class History {

public:
        History(const int items, const int length);
        ~History();
        void setMaxSize(const int items);
        int  getMaxSize();
        int  getCurrentSize();
        void addItem(const char *item);
        char *getCurrentItem();
        void prev();
        void next();
        bool isFirst();
        bool isLast();
        bool isNew();

private:
        char *buf;

        int len;
        int size;
        int currentSize;
        int current;

};

#endif // HISTORY_H
