#ifndef PLISTMANAGER_H
#define PLISTMANAGER_H

#include <string>
#include <vector>

class PlistManager
{
public:
    static void setPlistKey(const std::string& plistPath, const std::string& key, bool value);
private:
    static bool isBinaryPlist(const std::vector<char> &data);
};

#endif // PLISTMANAGER_H
