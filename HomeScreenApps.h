#ifndef HOMESCREENAPPS_H
#define HOMESCREENAPPS_H

#include "libimobiledevice/sbservices.h"
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>

class HomeScreenApps
{
public:
    static std::unordered_map<std::string, std::unordered_map<std::string, std::variant<std::string, std::vector<char>, int>>> getHomeScreenApps();
private:
    static void scoutArray(plist_t array, std::unordered_map<std::string, std::unordered_map<std::string, std::variant<std::string, std::vector<char>, int>>>& apps, bool writePos, sbservices_client_t sbservice_t);
    static void scoutAllApps(plist_t icon_state, std::unordered_map<std::string, std::unordered_map<std::string, std::variant<std::string, std::vector<char>, int>>>& apps, sbservices_client_t sbservice_t);
};

#endif // HOMESCREENAPPS_H
