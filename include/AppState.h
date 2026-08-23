#pragma once
#include <windows.h>
#include <vector> 
#include <atomic>

#define WM_REFRESH_UI (WM_USER + 3)

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

    std::atomic<bool> needsRedraw{true};

    bool keys[256] = { false };
    bool lmb = false, rmb = false, mmb = false; 
    bool isScrolling = false, mb4 = false, mb5 = false; 
    bool showExtraKey[256] = { false };
    bool showSpace = true, showShift = true, showCtrl = true;
    bool showMB4 = true, showMB5 = true;
    
    bool showCPS = false;
    std::vector<ULONGLONG> clickTimes; 
    
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

    int currentProfile = 0; 
    wchar_t globalIniPath[MAX_PATH] = {0};
    wchar_t profileIniPath[MAX_PATH] = {0};

    void InitPaths() {
        wchar_t baseDir[MAX_PATH];
        GetModuleFileNameW(NULL, baseDir, MAX_PATH);
        wchar_t* last = wcsrchr(baseDir, L'\\');
        if (last) *(last + 1) = L'\0';

        wcscpy_s(globalIniPath, MAX_PATH, baseDir); wcscat_s(globalIniPath, MAX_PATH, L"config.ini");
        wcscpy_s(profileIniPath, MAX_PATH, baseDir);
        if (currentProfile == 0) wcscat_s(profileIniPath, MAX_PATH, L"profile_default.ini");
        else if (currentProfile == 1) wcscat_s(profileIniPath, MAX_PATH, L"profile_1.ini");
        else if (currentProfile == 2) wcscat_s(profileIniPath, MAX_PATH, L"profile_2.ini");
        else if (currentProfile == 3) wcscat_s(profileIniPath, MAX_PATH, L"profile_3.ini");
    }

    void SaveConfig() {
        InitPaths();
        wchar_t buf[32]; wsprintfW(buf, L"%d", currentProfile);
        WritePrivateProfileStringW(L"Global", L"LastProfile", buf, globalIniPath);

        auto WriteInt = [&](const wchar_t* key, int val) {
            wsprintfW(buf, L"%d", val);
            WritePrivateProfileStringW(L"Settings", key, buf, profileIniPath);
        };
        
        WriteInt(L"baseAlpha", baseAlpha); WriteInt(L"highlightAlpha", highlightAlpha); WriteInt(L"outlineAlpha", outlineAlpha);
        WriteInt(L"activeBg", activeBg); WriteInt(L"inactiveBg", inactiveBg); WriteInt(L"activeText", activeText); WriteInt(L"inactiveText", inactiveText); WriteInt(L"outlineColor", outlineColor);
        WriteInt(L"uiScale", (int)(uiScale * 100)); WriteInt(L"currentScheme", currentScheme);
        WriteInt(L"showSpace", showSpace); WriteInt(L"showShift", showShift); WriteInt(L"showCtrl", showCtrl); 
        WriteInt(L"showMB4", showMB4); WriteInt(L"showMB5", showMB5); WriteInt(L"showCPS", showCPS); 
        
        wchar_t keyBuf[1024] = {0};
        for (int i = 0; i < 256; i++) {
            if (showExtraKey[i]) {
                wchar_t temp[16]; wsprintfW(temp, L"%d,", i);
                wcscat_s(keyBuf, 1024, temp);
            }
        }
        WritePrivateProfileStringW(L"Settings", L"ActiveKeys", keyBuf, profileIniPath);
        
        RECT rc; GetWindowRect(hwndOverlay, &rc);
        WriteInt(L"OverlayX", rc.left); WriteInt(L"OverlayY", rc.top);
    }

    void LoadConfig(bool isBoot = false) {
        if (isBoot) {
            InitPaths();
            currentProfile = GetPrivateProfileIntW(L"Global", L"LastProfile", 0, globalIniPath);
        }
        
        InitPaths(); 
        auto ReadInt = [&](const wchar_t* key, int defVal) { return GetPrivateProfileIntW(L"Settings", key, defVal, profileIniPath); };
        
        baseAlpha = ReadInt(L"baseAlpha", 80); highlightAlpha = ReadInt(L"highlightAlpha", 100); outlineAlpha = ReadInt(L"outlineAlpha", 0);
        activeBg = ReadInt(L"activeBg", RGB(0, 255, 204)); inactiveBg = ReadInt(L"inactiveBg", RGB(34, 34, 34)); activeText = ReadInt(L"activeText", RGB(0, 0, 0)); inactiveText = ReadInt(L"inactiveText", RGB(255, 255, 255)); outlineColor = ReadInt(L"outlineColor", RGB(255, 255, 255));
        uiScale = ReadInt(L"uiScale", 100) / 100.0f; currentScheme = ReadInt(L"currentScheme", 0);
        showSpace = ReadInt(L"showSpace", 1); showShift = ReadInt(L"showShift", 1); showCtrl = ReadInt(L"showCtrl", 1); 
        showMB4 = ReadInt(L"showMB4", 1); showMB5 = ReadInt(L"showMB5", 1); showCPS = ReadInt(L"showCPS", 0); 
        
        wchar_t keyBuf[1024] = {0};
        GetPrivateProfileStringW(L"Settings", L"ActiveKeys", L"", keyBuf, 1024, profileIniPath);
        
        for (int i = 0; i < 256; i++) showExtraKey[i] = false;
        showExtraKey['W'] = true; showExtraKey['A'] = true; showExtraKey['S'] = true; showExtraKey['D'] = true;

        if (wcslen(keyBuf) > 0) {
            wchar_t* context = NULL;
            wchar_t* token = wcstok_s(keyBuf, L",", &context);
            while (token) {
                int k = _wtoi(token);
                if (k >= 0 && k < 256) showExtraKey[k] = true;
                token = wcstok_s(NULL, L",", &context);
            }
        }
        clickTimes.clear(); 
        needsRedraw = true;
    }
};