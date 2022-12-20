#ifndef ENCODECHAR_H
#define ENCODECHAR_H

// Macros for CP1251 to/from Unicode converter
#define QT_NO_ASCII_CAST
#define QT_NO_CAST_ASCII

#include <QTextCodec>
#include <QString>

extern QTextCodec* Converter1251;

#define W2U(x) (Converter1251 == NULL ? QString::fromLatin1(x) : Converter1251->toUnicode(x))
#define U2W(x) (Converter1251 == NULL ? QByteArray(x.toLatin1()) : Converter1251->fromUnicode(x))

#endif // ENCODECHAR_H
