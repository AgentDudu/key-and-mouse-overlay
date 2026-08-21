#pragma once
#include <windows.h>

class AppState {
public:
    static AppState& Get() {
        static AppState instance;
        return instance;
    }

    bool keys[256] = { false };
    bool lmb = false;
    bool rmb = false;
    
    bool isLocked = true;
    float uiScale = 1.0f;
    const int BASE_WIDTH = 260;
    const int BASE_HEIGHT = 180;
    
    HWND hwndOverlay = NULL;
};