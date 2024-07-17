#include "util.h"

#include <QMimeDatabase>
#include <QFile>
#include <qDebug>
#include <QFileInfo>

Util::Util() {}

QString Util::getMimeType(const QString &filename)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filename);
    QStringList parts = type.name().split('/');
    if (parts.size() == 2) {
        return parts[1];
    }
    return QString();
}
QString Util::getFilename(const QString &filename){
    QFileInfo fileInfo(filename);
    return fileInfo.fileName();
}

QPair<bool,QString> Util::saveByteArray2File(const QByteArray data,const QString filename){
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error opening destination file: " << file.errorString();
        return QPair<bool,QString>(false,file.errorString());
    }
    file.write(data);
    file.close();
    return QPair<bool,QString>(true,"保存文件成功");
}
