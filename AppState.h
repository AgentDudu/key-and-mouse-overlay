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
    
    // Color Schemes (Default: Cyan)
    COLORREF activeBg = RGB(0, 255, 204);
    COLORREF inactiveBg = RGB(34, 34, 34);
    COLORREF activeText = RGB(0, 0, 0);
    COLORREF inactiveText = RGB(255, 255, 255);
    int currentScheme = 0; // 0: Cyan, 1: Red, 2: Green, 3: Custom

    HWND hwndOverlay = NULL;
};