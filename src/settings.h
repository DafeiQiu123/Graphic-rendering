#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    bool toggle1 = false;
    bool toggle2 = false;
    bool toggle3 = false;
    bool toggle4 = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
