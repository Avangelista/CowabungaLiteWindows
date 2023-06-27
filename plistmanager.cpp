#include "plistmanager.h"
#include <fstream>
#include <plist/plist++.h>

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

void PlistManager::setPlistKey(const std::string& plistPath, const std::string& key, bool value) {
    std::ifstream input(plistPath, std::ios::binary);
    if (!input.is_open()) {
        return;
    }
    std::vector<char> in((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
    input.close();

    auto bplist = isBinaryPlist(in);
    auto plist = PList::Dictionary();

    if (bplist) {
        plist = PList::Dictionary::FromBin(in);
    } else {
        plist = PList::Dictionary::FromXml(std::string(in.begin(), in.end()));
    }

    plist.Set(key, PList::Boolean(value));

    auto out = std::vector<char>();
    if (bplist) {
        out = plist.ToBin();
    } else {
        auto tmp = plist.ToXml();
        out = std::vector<char>(tmp.begin(), tmp.end());
    }

    std::ofstream output(plistPath, std::ios::binary);
    if (!output.is_open()) {
        return;
    }

    output.write(out.data(), out.size());
    output.close();
}
