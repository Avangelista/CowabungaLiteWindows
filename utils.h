#ifndef UTILS_H
#define UTILS_H

#include "qpixmap.h"
#include <QString>
#include <sstream>

class Utils
{
public:
    static bool copyDirectory(QString, QString);
    static void unzip(QString zipFile , QString outputFolder);
    static QPixmap createRoundedPixmap(const QPixmap& pixmap, double roundnessPercentage);
};

enum class Tweak {
    Themes,
    StatusBar,
    ControlCenter,
    SpringboardOptions,
    InternalOptions,
    SkipSetup
};

struct TweakData {
    std::string folderName;
    std::string description;
};

class Tweaks {
public:
    static TweakData getTweakData(Tweak tweak) {
        switch (tweak) {
        case Tweak::Themes:
            return {"Theme", "Icon Theming"};
        case Tweak::StatusBar:
            return {"StatusBar", "Status Bar"};
        case Tweak::ControlCenter:
            return {"ControlCenter", "Control Center"};
        case Tweak::SpringboardOptions:
            return {"SpringboardOptions", "Springboard Options"};
        case Tweak::InternalOptions:
            return {"InternalOptions", "Internal Options"};
        case Tweak::SkipSetup:
            return {"SkipSetup", "Setup Options"};
        default:
            return {"Unknown", "Unknown Tweak"};
        }
    }
};

class Version {
public:
    int major;
    int minor;
    int patch;

    Version() : major(0), minor(0), patch(0) {}

    Version(int major, int minor = 0, int patch = 0) {
        this->major = major;
        this->minor = minor;
        this->patch = patch;
    }

    Version(const std::string& versionString) {
        std::istringstream iss(versionString);
        char dot;
        iss >> major >> dot >> minor >> dot >> patch;
    }

    std::string toString() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }

    bool operator<(const Version& other) const {
        if (major < other.major)
            return true;
        if (major > other.major)
            return false;

        if (minor < other.minor)
            return true;
        if (minor > other.minor)
            return false;

        return patch < other.patch;
    }

    bool operator>(const Version& other) const {
        return other < *this;
    }

    bool operator<=(const Version& other) const {
        return !(other < *this);
    }

    bool operator>=(const Version& other) const {
        return !(*this < other);
    }

    bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
};

#endif // UTILS_H
