#pragma once
#include <windows.h>

class AppState {
public:
    static AppState& Get() {
        static AppState instance;
        return instance;
    }

    AppState() {
        showExtraKey['W'] = true; showExtraKey['A'] = true;
        showExtraKey['S'] = true; showExtraKey['D'] = true;
    }

    bool keys[256] = { false };
    bool lmb = false, rmb = false, mmb = false; 
    bool isScrolling = false, mb4 = false, mb5 = false; 
    bool showExtraKey[256] = { false };
    bool showSpace = true, showShift = true, showCtrl = true;
    bool showMB4 = true, showMB5 = true;
    
    bool isLocked = true;
    float uiScale = 1.0f;
    const int BASE_WIDTH = 310;
    int BASE_HEIGHT = 180;        
    int dynamicHeight = 180;      
    int dynamicWidth = 310;       
    
    COLORREF activeBg = RGB(0, 255, 204);
    COLORREF inactiveBg = RGB(34, 34, 34);
    COLORREF activeText = RGB(0, 0, 0);
    COLORREF inactiveText = RGB(255, 255, 255);
    
    int baseAlpha = 80;      
    int highlightAlpha = 100; 
    
    COLORREF outlineColor = RGB(255, 255, 255);
    int outlineAlpha = 0;
    
    int currentScheme = 0;

    HWND hwndOverlay = NULL;
    HWND hwndExtraKeys = NULL;    
};