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
    HFONT hFontUI = NULL;

    wchar_t iniPath[MAX_PATH] = {0};

    void InitPath() {
        if (iniPath[0] == 0) {
            GetModuleFileNameW(NULL, iniPath, MAX_PATH);
            wchar_t* last = wcsrchr(iniPath, L'\\');
            if (last) *(last + 1) = L'\0';
            wcscat_s(iniPath, MAX_PATH, L"config.ini");
        }
    }

    void SaveConfig() {
        InitPath();
        auto WriteInt = [&](const wchar_t* key, int val) {
            wchar_t buf[32]; wsprintfW(buf, L"%d", val);
            WritePrivateProfileStringW(L"Settings", key, buf, iniPath);
        };
        WriteInt(L"baseAlpha", baseAlpha); WriteInt(L"highlightAlpha", highlightAlpha); WriteInt(L"outlineAlpha", outlineAlpha);
        WriteInt(L"activeBg", activeBg); WriteInt(L"inactiveBg", inactiveBg); WriteInt(L"activeText", activeText); WriteInt(L"inactiveText", inactiveText); WriteInt(L"outlineColor", outlineColor);
        WriteInt(L"uiScale", (int)(uiScale * 100)); WriteInt(L"currentScheme", currentScheme);
        WriteInt(L"showSpace", showSpace); WriteInt(L"showShift", showShift); WriteInt(L"showCtrl", showCtrl); WriteInt(L"showMB4", showMB4); WriteInt(L"showMB5", showMB5);
        
        wchar_t keyBuf[1024] = {0};
        for (int i = 0; i < 256; i++) {
            if (showExtraKey[i]) {
                wchar_t temp[16]; wsprintfW(temp, L"%d,", i);
                wcscat_s(keyBuf, 1024, temp);
            }
        }
        WritePrivateProfileStringW(L"Settings", L"ActiveKeys", keyBuf, iniPath);
        
        RECT rc; GetWindowRect(hwndOverlay, &rc);
        WriteInt(L"OverlayX", rc.left); WriteInt(L"OverlayY", rc.top);
    }

    void LoadConfig() {
        InitPath();
        auto ReadInt = [&](const wchar_t* key, int defVal) { return GetPrivateProfileIntW(L"Settings", key, defVal, iniPath); };
        
        if (ReadInt(L"baseAlpha", -1) == -1) return; 

        baseAlpha = ReadInt(L"baseAlpha", 80); highlightAlpha = ReadInt(L"highlightAlpha", 100); outlineAlpha = ReadInt(L"outlineAlpha", 0);
        activeBg = ReadInt(L"activeBg", RGB(0, 255, 204)); inactiveBg = ReadInt(L"inactiveBg", RGB(34, 34, 34)); activeText = ReadInt(L"activeText", RGB(0, 0, 0)); inactiveText = ReadInt(L"inactiveText", RGB(255, 255, 255)); outlineColor = ReadInt(L"outlineColor", RGB(255, 255, 255));
        uiScale = ReadInt(L"uiScale", 100) / 100.0f; currentScheme = ReadInt(L"currentScheme", 0);
        showSpace = ReadInt(L"showSpace", 1); showShift = ReadInt(L"showShift", 1); showCtrl = ReadInt(L"showCtrl", 1); showMB4 = ReadInt(L"showMB4", 1); showMB5 = ReadInt(L"showMB5", 1);
        
        wchar_t keyBuf[1024] = {0};
        GetPrivateProfileStringW(L"Settings", L"ActiveKeys", L"", keyBuf, 1024, iniPath);
        
        if (wcslen(keyBuf) > 0) {
            for (int i = 0; i < 256; i++) showExtraKey[i] = false;
            wchar_t* context = NULL;
            wchar_t* token = wcstok_s(keyBuf, L",", &context);
            while (token) {
                int k = _wtoi(token);
                if (k >= 0 && k < 256) showExtraKey[k] = true;
                token = wcstok_s(NULL, L",", &context);
            }
        } else {
            for (int i = 0; i < 256; i++) {
                wchar_t kName[16]; wsprintfW(kName, L"Key_%d", i);
                showExtraKey[i] = ReadInt(kName, showExtraKey[i]);
            }
        }
    }
};