#ifndef HOMESCREENAPPS_H
#define HOMESCREENAPPS_H

#include "libimobiledevice/sbservices.h"
#include "plist/Dictionary.h"
#include "plist/Node.h"
class HomeScreenApps
{
public:
    static PList::Dictionary* getHomeScreenApps();
private:
    static void scoutArray(plist_t array, plist_t apps, bool writePos, sbservices_client_t sbservice_t);
    static void scoutAllApps(plist_t icon_state, plist_t apps, sbservices_client_t sbservice_t);
};

#endif // HOMESCREENAPPS_H
