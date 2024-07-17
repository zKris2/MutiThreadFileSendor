#ifndef UTIL_H
#define UTIL_H
#include <QString>

class Util
{
public:
    Util();
    static QString getMimeType(const QString &filename);
    static QString getFilename(const QString &filename);
    static QPair<bool,QString> saveByteArray2File(const QByteArray data,const QString filename);
};

#endif // UTIL_H
