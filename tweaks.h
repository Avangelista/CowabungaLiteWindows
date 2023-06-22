#ifndef TWEAKS_H
#define TWEAKS_H

#include <string>

enum class Tweak {
    AppliedTheme,
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
        case Tweak::AppliedTheme:
            return {"AppliedTheme", "Icon Theming"};
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

#endif // TWEAKS_H
