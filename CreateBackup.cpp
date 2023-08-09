// Rory Madden 2023

// compile: g++ -o CreateBackup CreateBackup.cpp -lcrypto

#include "CreateBackup.h"
#include "qdir.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <cstdint>
#include <filesystem>
#include <regex>
#include <openssl/sha.h>
#include <random>
#include <QString>
#include <QDebug>
#include <QRandomGenerator>
#include <QCryptographicHash>

std::string removeDomain(const std::string &domain, const std::string &input)
{
    size_t pos = input.find(domain);
    if (pos != std::string::npos)
    {
        pos += domain.length();
        if (input[pos] == '\\' || input[pos] == '/')
        {
            ++pos;
        }
        return input.substr(pos);
    }
    return input;
}

void writeStringWithLength(QFile &output_file, const QString &qstr)
{
    QByteArray byteArray = qstr.toUtf8();
    quint16 length = static_cast<quint16>(byteArray.size());

    quint16 bigEndianLength = (length << 8) | (length >> 8);

    output_file.write(reinterpret_cast<const char *>(&bigEndianLength), sizeof(bigEndianLength));
    output_file.write(byteArray);
}

void writeHash(QFile &output_file, const QString &file)
{
    QFile input_file(file);
    if (!input_file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file:" << file;
        return;
    }

    QByteArray fileContent = input_file.readAll();
    QByteArray hash = QCryptographicHash::hash(fileContent, QCryptographicHash::Sha1);
    output_file.write(hash);
}

void generateRandomHex(QFile &output_file)
{
    QRandomGenerator generator;
    generator.seed(QDateTime::currentMSecsSinceEpoch());

    constexpr int bufferSize = 12;
    QByteArray buffer;
    buffer.resize(bufferSize);

    for (int i = 0; i < bufferSize; ++i)
    {
        buffer[i] = static_cast<char>(generator.generate() % 256);
    }

    output_file.write(buffer);

    // For testing purposes
    // QByteArray testData(12, '\xFF');
    // output_file.write(testData);
}

std::string calculateSHA1(const std::string &str)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(str.c_str()), str.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

void copyFile(const QString &source, const QString &destination)
{
    QFile sourceFile(source);
    if (!sourceFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open source file:" << source;
        return;
    }

    QFile destinationFile(destination);
    if (!destinationFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to create destination file:" << destination;
        return;
    }

    destinationFile.write(sourceFile.readAll());
}

void processFiles(const std::string &path, const std::string &domainString, const std::string &outputDir)
{
    QString qPath = QString::fromStdString(path);
    QString qDomainString = QString::fromStdString(domainString);
    QString qOutputDir = QString::fromStdString(outputDir);

    QString fileString = QString::fromStdString(removeDomain(qDomainString.toStdString(), qPath.toStdString()))
                            .replace("hiddendot", ".")
                            .replace("\\", "/");
    
    QFile output_file(qOutputDir + "/Manifest.mbdb");
    if (!output_file.open(QIODevice::Append))
    {
        qDebug() << "Failed to open output file";
        return;
    }

    if (qDomainString == "ConfigProfileDomain")
    {
        writeStringWithLength(output_file, "SysSharedContainerDomain-systemgroup.com.apple.configurationprofiles");
    }
    else
    {
        writeStringWithLength(output_file, qDomainString);
    }
    writeStringWithLength(output_file, fileString);

    if (QFileInfo(qPath).isFile())
    {
        output_file.write("\xFF\xFF\x00\x14", 4);
        writeHash(output_file, qPath);
        output_file.write("\xFF\xFF\x81\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\xF5\x00\x00\x01\xF5", 20);
        generateRandomHex(output_file);
        QFileInfo fileInfo(qPath);
        qint64 fileSize = fileInfo.size();
        QByteArray sizeBytes;

        // Convert to big endian
        for (int i = 7; i >= 0; --i)
        {
            unsigned char byte = static_cast<unsigned char>((fileSize >> (8 * i)) & 0xFF);
            sizeBytes.append(byte);
        }

        output_file.write(sizeBytes);
        output_file.write("\x04\x00", 2);
        output_file.close();

        QString hash;
        if (qDomainString == "ConfigProfileDomain")
        {
            hash = QString::fromStdString(calculateSHA1("SysSharedContainerDomain-systemgroup.com.apple.configurationprofiles-" + fileString.toStdString()));
        }
        else
        {
            hash = QString::fromStdString(calculateSHA1(domainString + "-" + fileString.toStdString()));
        }
        QString newFile = qOutputDir + "/" + hash;
        copyFile(qPath, newFile);
    }
    else if (QFileInfo(qPath).isDir())
    {
        output_file.write("\xFF\xFF\xFF\xFF\xFF\xFF\x41\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\xF5\x00\x00\x01\xF5", 24);
        generateRandomHex(output_file);
        output_file.write("\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00", 10);
        output_file.close();

        QDir dir(qPath);
        for (const QString &entry : dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
        {
            QString filePath = qPath + "/" + entry;
            processFiles(filePath.toStdString(), qDomainString.toStdString(), qOutputDir.toStdString());
        }
    }
}

std::string basename(const std::string &path)
{
    size_t lastSlash = path.find_last_of("/\\");
    std::string filename = path.substr(lastSlash + 1);
    return filename;
}

bool createDirectory(const std::string &dirPath)
{
    QString qDirPath = QString::fromStdString(dirPath);

    try
    {
        if (QDir().mkpath(qDirPath))
            return true;
        else
        {
            qDebug() << "Failed to create directory.";
            return false;
        }
    }
    catch (const std::exception &ex)
    {
        qDebug() << "Failed to create directory:" << ex.what();
        return false;
    }
}

bool removeDirectoryIfExists(const std::string &dirPath)
{
    QString qDirPath = QString::fromStdString(dirPath);

    if (QFileInfo(qDirPath).exists() && QFileInfo(qDirPath).isDir())
    {
        try
        {
            if (QDir(qDirPath).removeRecursively())
                return true;
            else
                return false;
        }
        catch (const std::exception &ex)
        {
            qDebug() << "Failed to remove directory:" << ex.what();
            return false;
        }
    }
    return false;
}

bool CreateBackup::createBackup(std::string indir, std::string outdir)
{
    removeDirectoryIfExists(outdir);
    createDirectory(outdir);

    QString qIndir = QString::fromStdString(indir);
    QString qOutdir = QString::fromStdString(outdir);

    // NOTE: Manifest.mbdb tracks the locations and SHA1 hashes of each file in the backup
    QFile output_file(qOutdir + "/Manifest.mbdb");
    if (!output_file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to create output file";
        return false;
    }

    // Manifest.mbdb file header
    QByteArray header = "mbdb\x05\x00";
    output_file.write(header, 6);

    // Close the file after writing the header
    output_file.close();

    // Iterate over all domains
    QDir domainDir(qIndir);
    for (const QString &domainEntry : domainDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString domain = qIndir + "/" + domainEntry;
        QString domainString = QFileInfo(domain).baseName();

        processFiles(domain.toStdString(), domainString.toStdString(), outdir);
    }

    // Generate Info.plist
    std::ofstream infoPlist(outdir + "/Info.plist", std::ios::binary);
    infoPlist << R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
</dict>
</plist>
)";
    infoPlist.close();

    // Generate Status.plist
    std::ofstream statusPlist(outdir + "/Status.plist", std::ios::binary);
    statusPlist << R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>BackupState</key>
	<string>new</string>
	<key>Date</key>
	<date>1970-01-01T00:00:00Z</date>
	<key>IsFullBackup</key>
	<false/>
	<key>SnapshotState</key>
	<string>finished</string>
	<key>UUID</key>
	<string>00000000-0000-0000-0000-000000000000</string>
	<key>Version</key>
	<string>2.4</string>
</dict>
</plist>
)";
    statusPlist.close();

    // Generate Manifest.plist
    std::ofstream manifestPlist(outdir + "/Manifest.plist", std::ios::binary);
    manifestPlist << R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>BackupKeyBag</key>
	<data>
	VkVSUwAAAAQAAAAFVFlQRQAAAAQAAAABVVVJRAAAABDud41d1b9NBICR1BH9JfVtSE1D
	SwAAACgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAV1JBUAAA
	AAQAAAAAU0FMVAAAABRY5Ne2bthGQ5rf4O3gikep1e6tZUlURVIAAAAEAAAnEFVVSUQA
	AAAQB7R8awiGR9aba1UuVahGPENMQVMAAAAEAAAAAVdSQVAAAAAEAAAAAktUWVAAAAAE
	AAAAAFdQS1kAAAAoN3kQAJloFg+ukEUY+v5P+dhc/Welw/oucsyS40UBh67ZHef5ZMk9
	UVVVSUQAAAAQgd0cg0hSTgaxR3PVUbcEkUNMQVMAAAAEAAAAAldSQVAAAAAEAAAAAktU
	WVAAAAAEAAAAAFdQS1kAAAAoMiQTXx0SJlyrGJzdKZQ+SfL124w+2Tf/3d1R2i9yNj9z
	ZCHNJhnorVVVSUQAAAAQf7JFQiBOS12JDD7qwKNTSkNMQVMAAAAEAAAAA1dSQVAAAAAE
	AAAAAktUWVAAAAAEAAAAAFdQS1kAAAAoSEelorROJA46ZUdwDHhMKiRguQyqHukotrxh
	jIfqiZ5ESBXX9txi51VVSUQAAAAQfF0G/837QLq01xH9+66vx0NMQVMAAAAEAAAABFdS
	QVAAAAAEAAAAAktUWVAAAAAEAAAAAFdQS1kAAAAol0BvFhd5bu4Hr75XqzNf4g0fMqZA
	ie6OxI+x/pgm6Y95XW17N+ZIDVVVSUQAAAAQimkT2dp1QeadMu1KhJKNTUNMQVMAAAAE
	AAAABVdSQVAAAAAEAAAAA0tUWVAAAAAEAAAAAFdQS1kAAAAo2N2DZarQ6GPoWRgTiy/t
	djKArOqTaH0tPSG9KLbIjGTOcLodhx23xFVVSUQAAAAQQV37JVZHQFiKpoNiGmT6+ENM
	QVMAAAAEAAAABldSQVAAAAAEAAAAA0tUWVAAAAAEAAAAAFdQS1kAAAAofe2QSvDC2cV7
	Etk4fSBbgqDx5ne/z1VHwmJ6NdVrTyWi80Sy869DM1VVSUQAAAAQFzkdH+VgSOmTj3yE
	cfWmMUNMQVMAAAAEAAAAB1dSQVAAAAAEAAAAA0tUWVAAAAAEAAAAAFdQS1kAAAAo7kLY
	PQ/DnHBERGpaz37eyntIX/XzovsS0mpHW3SoHvrb9RBgOB+WblVVSUQAAAAQEBpgKOz9
	Tni8F9kmSXd0sENMQVMAAAAEAAAACFdSQVAAAAAEAAAAA0tUWVAAAAAEAAAAAFdQS1kA
	AAAo5mxVoyNFgPMzphYhm1VG8Fhsin/xX+r6mCd9gByF5SxeolAIT/ICF1VVSUQAAAAQ
	rfKB2uPSQtWh82yx6w4BoUNMQVMAAAAEAAAACVdSQVAAAAAEAAAAA0tUWVAAAAAEAAAA
	AFdQS1kAAAAo5iayZBwcRa1c1MMx7vh6lOYux3oDI/bdxFCW1WHCQR/Ub1MOv+QaYFVV
	SUQAAAAQiLXvK3qvQza/mea5inss/0NMQVMAAAAEAAAACldSQVAAAAAEAAAAA0tUWVAA
	AAAEAAAAAFdQS1kAAAAoD2wHX7KriEe1E31z7SQ7/+AVymcpARMYnQgegtZD0Mq2U55u
	xwNr2FVVSUQAAAAQ/Q9feZxLS++qSe/a4emRRENMQVMAAAAEAAAAC1dSQVAAAAAEAAAA
	A0tUWVAAAAAEAAAAAFdQS1kAAAAocYda2jyYzzSKggRPw/qgh6QPESlkZedgDUKpTr4Z
	Z8FDgd7YoALY1g==
	</data>
	<key>Lockdown</key>
	<dict/>
	<key>SystemDomainsVersion</key>
	<string>20.0</string>
	<key>Version</key>
	<string>9.1</string>
</dict>
</plist>
)";
    manifestPlist.close();

    return true;
}
