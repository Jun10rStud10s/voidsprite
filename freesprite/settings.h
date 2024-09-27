#pragma once
#include "mathops.h"

struct GlobalConfig {
    bool openSavedPath = true;
    bool animatedBackground = true;
    int maxUndoHistory = 20;
    bool scrollWithTouchpad = false;
};

inline GlobalConfig g_config;

inline bool g_saveConfig() {
    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ofstream file(path);
    if (file.is_open()) {
        file << "openSavedPath=" << (g_config.openSavedPath ? "1" : "0") << std::endl;
        file << "animatedBackground=" << (g_config.animatedBackground ? "1" : "0") << std::endl;
        file << "maxUndoHistory=" << std::to_string(g_config.maxUndoHistory) << std::endl;
        file << "scrollWithTouchpad=" << (g_config.scrollWithTouchpad ? "1" : "0") << std::endl;
        file.close();
        return true;
    }
    else {
        return false;
    }
}

inline void g_loadConfig() {
    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ifstream file(path);
    if (file.is_open()) {
        std::map<std::string, std::string> config;
        std::string line;
        while (std::getline(file, line)) {
            std::string key = line.substr(0, line.find("="));
            std::string value = line.substr(line.find("=") + 1);
            config[key] = value;
        }

        if (config.contains("openSavedPath")) { g_config.openSavedPath = config["openSavedPath"] == "1"; }
        if (config.contains("animatedBackground")) { g_config.animatedBackground = config["animatedBackground"] == "1"; }
        if (config.contains("maxUndoHistory")) { try { g_config.maxUndoHistory = std::stoi(config["maxUndoHistory"]); } catch (std::exception) {} }
        if (config.contains("scrollWithTouchpad")) { g_config.scrollWithTouchpad = config["scrollWithTouchpad"] == "1"; }

        file.close();
    }
}