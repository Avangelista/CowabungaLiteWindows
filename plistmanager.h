#ifndef PLISTMANAGER_H
#define PLISTMANAGER_H

#include <string>
#include <vector>
#include <plist/plist++.h>
#include <QString>

class PlistManager
{
public:
    static PList::Node* getPlistValue(const QString& plistPath, const std::string& key);
    static void setPlistValue(const QString& plistPath, const std::string& key, PList::Node& value);
    static void deletePlistKey(const QString& plistPath, const std::string &key);
    static void createEmptyPlist(const QString& plistPath, bool bplist);
private:
    static bool isBinaryPlist(const std::vector<char> &data);
};

#endif // PLISTMANAGER_H
