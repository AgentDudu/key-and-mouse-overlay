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
    bool mmb = false; 
    bool isScrolling = false; 
    bool mb4 = false; 
    bool mb5 = false; 
    
    bool isLocked = true;
    float uiScale = 1.0f;
    const int BASE_WIDTH = 310;
    const int BASE_HEIGHT = 180;
    
    COLORREF activeBg = RGB(0, 255, 204);
    COLORREF inactiveBg = RGB(34, 34, 34);
    COLORREF activeText = RGB(0, 0, 0);
    COLORREF inactiveText = RGB(255, 255, 255);
    int currentScheme = 0;

    HWND hwndOverlay = NULL;
};