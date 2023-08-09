#include "utils.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <zlib.h>

bool Utils::copyDirectory(QString source, QString dest) {
    QDir().mkpath(dest);

    auto iterator = QDirIterator(source, QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (iterator.hasNext()) {
        iterator.next();

        auto sourceFilePath = iterator.filePath();
        auto relativeFilePath = iterator.fileInfo().absoluteFilePath().replace(source, "");
        auto destinationFilePath = dest + relativeFilePath;

        // Create the destination directory path if necessary
        QDir().mkpath(QFileInfo(destinationFilePath).absolutePath());

        // Copy the file or directory
        if (iterator.fileInfo().isFile()) {
            if (!QFile::copy(sourceFilePath, destinationFilePath)) {
//                return false;
            }
        } else if (iterator.fileInfo().isDir()) {
            if (!QDir().mkdir(destinationFilePath)) {
//                return false;
            }
        }
    }

    return true;
}

void Utils::unzip (QString zipFile , QString outputFolder){
    QStringList arguments;
    arguments << "-x" << "-d" << outputFolder << zipFile;

    QProcess process;
    process.start("minizip.exe", arguments);
    process.waitForFinished(-1);

    QByteArray output = process.readAllStandardOutput();
    QByteArray errorOutput = process.readAllStandardError();

    qDebug() << "Standard Output:" << output;
    qDebug() << "Error Output:" << errorOutput;
}
