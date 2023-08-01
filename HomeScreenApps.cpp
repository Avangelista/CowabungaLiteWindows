#include "HomeScreenApps.h"
#include "plist/Dictionary.h"
#include "plist/Node.h"
#include <iostream>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/sbservices.h>
#include <plist/plist.h>
#include <string>
#include <vector>
#include "DeviceManager.h"
#include "qmessagebox.h"

void HomeScreenApps::scoutArray(plist_t array, plist_t apps, bool writePos, sbservices_client_t sbservice_t)
{
    int num_items = plist_array_get_size(array);
    for (int j = 0; j < num_items; j++)
    {
        plist_t dict = plist_array_get_item(array, j);
        plist_t displayName = plist_dict_get_item(dict, "displayName");
        if (displayName)
        {
            plist_t bundleIdentifier = plist_dict_get_item(dict, "bundleIdentifier");
            plist_t displayIdentifier = plist_dict_get_item(dict, "displayIdentifier");

            // Regular app
            if (bundleIdentifier)
            {
                char *name = nullptr;
                plist_get_string_val(displayName, &name);
                char *bundle = nullptr;
                plist_get_string_val(bundleIdentifier, &bundle);

                plist_t currentApp = plist_dict_get_item(apps, bundle);
                if (currentApp == nullptr) {
                    currentApp = plist_new_dict();
                    plist_dict_set_item(apps, bundle, currentApp);
                    plist_dict_set_item(currentApp, "name", plist_new_string(name));
                }
                plist_dict_set_item(currentApp, "icon_position", plist_new_int(writePos ? j : -1));

                char* pngdata = nullptr;
                uint64_t pngsize = 0;
                sbservices_error_t sb_icon_err_code = sbservices_get_icon_pngdata(sbservice_t, bundle, &pngdata, &pngsize);
                if (sb_icon_err_code == SBSERVICES_E_SUCCESS)
                {
                    plist_dict_set_item(currentApp, "icon", plist_new_data(pngdata, pngsize));
                }
            }
            // Themed app
            else if (displayIdentifier)
            {
                char *identifier = nullptr;
                plist_get_string_val(displayIdentifier, &identifier);

                int prefix_len = strlen("Cowabunga_");
                if (strncmp(identifier, "Cowabunga_", prefix_len) == 0) {
                    char bundle[256];
                    char name[256];

                    sscanf(identifier, "Cowabunga_%[^,],%[^\n]", bundle, name);

                    plist_t currentApp = plist_dict_get_item(apps, bundle);
                    if (currentApp == nullptr) {
                        currentApp = plist_new_dict();
                        plist_dict_set_item(apps, bundle, currentApp);
                        plist_dict_set_item(currentApp, "name", plist_new_string(name));
                    }
                    plist_dict_set_item(currentApp, "themed_icon_position", plist_new_int(writePos ? j : -1));

                    char* pngdata = nullptr;
                    uint64_t pngsize = 0;
                    std::cout << identifier << std::endl;
                    sbservices_error_t sb_icon_err_code = sbservices_get_icon_pngdata(sbservice_t, identifier, &pngdata, &pngsize);
                    if (sb_icon_err_code == SBSERVICES_E_SUCCESS)
                    {
                        plist_dict_set_item(currentApp, "themed_icon", plist_new_data(pngdata, pngsize));
                    }
                }
            }
            else
            {
                plist_t listType = plist_dict_get_item(dict, "listType");
                if (listType)
                {
                    char *type = nullptr;
                    plist_get_string_val(listType, &type);
                    // Folder
                    if (strcmp("folder", type) == 0)
                    {
                        plist_t iconLists = plist_dict_get_item(dict, "iconLists");
                        if (iconLists)
                        {
                            int size = plist_array_get_size(iconLists);
                            for (int i = 0; i < size; i++)
                            {
                                plist_t iconListsArray = plist_array_get_item(iconLists, i);
                                scoutArray(iconListsArray, apps, false, sbservice_t);
                            }
                        }
                    }
                }
            }
        }
    }
}

void HomeScreenApps::scoutAllApps(plist_t icon_state, plist_t apps, sbservices_client_t sbservice_t)
{
    int size = plist_array_get_size(icon_state);
    for (int i = 0; i < size; i++)
    {
        plist_t array = plist_array_get_item(icon_state, i);
        HomeScreenApps::scoutArray(array, apps, true, sbservice_t);
    }
}

PList::Dictionary* HomeScreenApps::getHomeScreenApps()
{
    auto udid = DeviceManager::getInstance().getCurrentUUID();
    if (!udid) {
        return nullptr;
    }

    idevice_error_t idevice_ret = IDEVICE_E_UNKNOWN_ERROR;
    idevice_t device;
    plist_t icon_state;
    sbservices_client_t sbservice_t;

    // Get device
    idevice_ret = idevice_new(&device, udid->c_str());
    if (idevice_ret != IDEVICE_E_SUCCESS)
    {
        std::cout << "No device found with UDID " << *udid << std::endl;
        return nullptr;
    }

    // Get client
    sbservices_error_t sb_client_err_code = sbservices_client_start_service(device, &sbservice_t, "iPhone");
    if (sb_client_err_code != SBSERVICES_E_SUCCESS)
    {
        // possible itunes error!
        QMessageBox::critical(nullptr, "Error", "Unable to read the list of apps from your phone. Please make sure iTunes is installed on this PC.");
        std::cerr << "Unable to create SpringBoard client: " << sb_client_err_code << std::endl;
        return nullptr;
    }

    // Get icon state
    sbservices_error_t sb_icon_err_code = sbservices_get_icon_state(sbservice_t, &icon_state, "2");
    if (sb_icon_err_code != SBSERVICES_E_SUCCESS)
    {
        std::cerr << "Unable to get icon state: " << sb_icon_err_code << std::endl;
    }

    plist_t apps = plist_new_dict();
    HomeScreenApps::scoutAllApps(icon_state, apps, sbservice_t);

    return dynamic_cast<PList::Dictionary *>(PList::Node::FromPlist(apps));
}
