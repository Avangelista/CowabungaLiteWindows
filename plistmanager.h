#ifndef PLISTMANAGER_H
#define PLISTMANAGER_H

#include <string>
#include <vector>
#include <plist/plist++.h>

class PlistManager
{
public:
    static PList::Node* getPlistValue(const std::string& plistPath, const std::string& key);
    static void setPlistValue(const std::string& plistPath, const std::string& key, PList::Node& value);
    static void deletePlistKey(const std::string &plistPath, const std::string &key);
private:
    static bool isBinaryPlist(const std::vector<char> &data);
};

#endif // PLISTMANAGER_H
