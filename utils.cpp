#include "utils.h"
#include "qpainter.h"
#include "qpainterpath.h"
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

// Helper function to create rounded pixmap
QPixmap Utils::createRoundedPixmap(const QPixmap& pixmap, double roundnessPercentage)
{
    QPixmap roundedPixmap(pixmap.size());
    roundedPixmap.fill(Qt::transparent);

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int width = pixmap.width();
    int height = pixmap.height();
    int radius = qMin(width, height) * roundnessPercentage;

    QPainterPath path;
    path.addRoundedRect(roundedPixmap.rect(), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pixmap);

    return roundedPixmap;
}

void Utils::unzip (QString zipFile , QString outputFolder, bool expandFolders){
    QDir outputDir(outputFolder);

    // Check if the output folder exists and delete it if it does
    if (outputDir.exists()) {
        qDebug() << "Output folder already exists. Deleting...";
        QDir(outputFolder).removeRecursively();
    }

    QStringList arguments;
    arguments << "-x" << "-d" << outputFolder << zipFile;

    QProcess process;
    process.start("minizip.exe", arguments);
    process.waitForFinished(-1);

    QByteArray output = process.readAllStandardOutput();
    QByteArray errorOutput = process.readAllStandardError();

    qDebug() << "Standard Output:" << output;
    qDebug() << "Error Output:" << errorOutput;

    // check if there was a folder in the zip

    if (expandFolders) {
        if (!outputDir.exists()) {
            qWarning() << "Input folder does not exist.";
            return;
        }

        QStringList subdirs = outputDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        foreach (const QString &subfolderName, subdirs) {
            if (subfolderName != "__MACOSX") { // Exclude __MACOSX
                QDir subfolderDir(outputDir.filePath(subfolderName));
                QStringList fileNames = subfolderDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

                foreach (const QString &fileName, fileNames) {
                QString srcPath = subfolderDir.filePath(fileName);
                QString destPath = outputDir.filePath(fileName);

                if (QFileInfo(srcPath).isDir()) {
                    QDir().rename(srcPath, destPath);
                } else {
                    QFile::copy(srcPath, destPath);
                }
                }

                subfolderDir.removeRecursively();
            }
        }
    }
}
