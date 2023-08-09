#include "plistmanager.h"
#include <fstream>
#include <QDebug>
#include <iostream>
#include <QFile>

bool PlistManager::isBinaryPlist(const std::vector<char>& data) {
    if (data.size() < 8) {
        return false;
    }

    const char expectedSignature[] = { 'b', 'p', 'l', 'i', 's', 't', '0', '0' };

    for (int i = 0; i < 8; i++) {
        if (data[i] != expectedSignature[i]) {
            return false;
        }
    }

    return true;
}

PList::Node* PlistManager::getPlistValue(const QString& plistPath, const std::string& key) {
//    std::ifstream input(plistPath, std::ios::binary);
//    if (!input.is_open()) {
//        return nullptr;
//    }
//    std::vector<char> in((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
//    input.close();

    std::vector<char> in;
    QFile inputFile(plistPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QByteArray byteArray = inputFile.readAll();
    in.resize(byteArray.size());
    std::memcpy(in.data(), byteArray.data(), byteArray.size());

    inputFile.close();

    auto bplist = isBinaryPlist(in);
    plist_t root = NULL;

    if (bplist) {
        plist_from_bin(&in[0], in.size(), &root);
    } else {
        plist_from_xml(&in[0], in.size(), &root);
    }

    plist_t value = plist_dict_get_item(root, key.c_str());

    return PList::Node::FromPlist(value);
}

void PlistManager::setPlistValue(const QString& plistPath, const std::string& key, PList::Node& value) {
//    std::ifstream input(plistPath, std::ios::binary);
//    if (!input.is_open()) {
//        return;
//    }
//    std::vector<char> in((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
//    input.close();

    std::vector<char> in;
    QFile inputFile(plistPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray byteArray = inputFile.readAll();
    in.resize(byteArray.size());
    std::memcpy(in.data(), byteArray.data(), byteArray.size());

    inputFile.close();

    auto bplist = isBinaryPlist(in);
    plist_t root = NULL;

    if (bplist) {
        plist_from_bin(&in[0], in.size(), &root);
    } else {
        plist_from_xml(&in[0], in.size(), &root);
    }

    plist_dict_set_item(root, key.c_str(), value.GetPlist());

    auto out = std::vector<char>();
    if (bplist) {
        char* bin = NULL;
        uint32_t length = 0;
        plist_to_bin(root, &bin, &length);
        std::vector<char> tmp(bin, bin+length);
        delete bin;
        out = tmp;
    } else {
        char* xml = NULL;
        uint32_t length = 0;
        plist_to_xml(root, &xml, &length);
        std::string tmp(xml, xml+length);
        delete xml;
        out = std::vector<char>(tmp.begin(), tmp.end());
    }

//    std::ofstream output(plistPath, std::ios::binary);
//    if (!output.is_open()) {
//        return;
//    }

//    output.write(out.data(), out.size());
//    output.close();

    QFile file(plistPath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream outStream(&file);
        outStream.writeRawData(out.data(), out.size());
        file.close();
    }
}

void PlistManager::deletePlistKey(const QString& plistPath, const std::string& key) {
//    std::ifstream input(plistPath, std::ios::binary);
//    if (!input.is_open()) {
//        return;
//    }
//    std::vector<char> in((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
//    input.close();

    std::vector<char> in;
    QFile inputFile(plistPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray byteArray = inputFile.readAll();
    in.resize(byteArray.size());
    std::memcpy(in.data(), byteArray.data(), byteArray.size());

    inputFile.close();

    auto bplist = isBinaryPlist(in);
    plist_t root = NULL;

    if (bplist) {
        plist_from_bin(&in[0], in.size(), &root);
    } else {
        plist_from_xml(&in[0], in.size(), &root);
    }

    plist_dict_remove_item(root, key.c_str());

    auto out = std::vector<char>();
    if (bplist) {
        char* bin = NULL;
        uint32_t length = 0;
        plist_to_bin(root, &bin, &length);
        std::vector<char> tmp(bin, bin+length);
        delete bin;
        out = tmp;
    } else {
        char* xml = NULL;
        uint32_t length = 0;
        plist_to_xml(root, &xml, &length);
        std::string tmp(xml, xml+length);
        delete xml;
        out = std::vector<char>(tmp.begin(), tmp.end());
    }

//    std::ofstream output(plistPath, std::ios::binary);
//    if (!output.is_open()) {
//        return;
//    }

//    output.write(out.data(), out.size());
//    output.close();

    QFile file(plistPath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream outStream(&file);
        outStream.writeRawData(out.data(), out.size());
        file.close();
    }
}

void PlistManager::createEmptyPlist(const QString& plistPath, bool bplist) {
    plist_t root = plist_new_dict();

    auto out = std::vector<char>();
    if (bplist) {
        char* bin = NULL;
        uint32_t length = 0;
        plist_to_bin(root, &bin, &length);
        std::vector<char> tmp(bin, bin+length);
        delete bin;
        out = tmp;
    } else {
        char* xml = NULL;
        uint32_t length = 0;
        plist_to_xml(root, &xml, &length);
        std::string tmp(xml, xml+length);
        delete xml;
        out = std::vector<char>(tmp.begin(), tmp.end());
    }

//    std::ofstream output(plistPath, std::ios::binary);
//    if (!output.is_open()) {
//        return;
//    }

//    output.write(out.data(), out.size());
//    output.close();

    QFile file(plistPath);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream outStream(&file);
        outStream.writeRawData(out.data(), out.size());
        file.close();
    }
}
