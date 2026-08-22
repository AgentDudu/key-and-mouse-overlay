#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h> 
#include "AppState.h"
#include "InputManager.h"
#include "OverlayUI.h"
#include "ExtraKeysUI.h"
#include "ControlUI.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib") 
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    AppState::Get().LoadConfig();

    INITCOMMONCONTROLSEX icex; icex.dwSize = sizeof(INITCOMMONCONTROLSEX); icex.dwICC = ICC_STANDARD_CLASSES; InitCommonControlsEx(&icex);
    AppState::Get().hFontUI = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    Gdiplus::GdiplusStartupInput gdiplusStartupInput; ULONG_PTR gdiplusToken; Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    AppState::Get().hwndOverlay = OverlayUI::Create(hInstance);
    HWND hwndControl = ControlUI::Create(hInstance);
    ExtraKeysUI::Init(hInstance);

    InputManager::Install(hInstance);

    int savedX = GetPrivateProfileIntW(L"Settings", L"OverlayX", -1, AppState::Get().iniPath);
    int savedY = GetPrivateProfileIntW(L"Settings", L"OverlayY", -1, AppState::Get().iniPath);
    if (savedX != -1 && savedY != -1) {
        SetWindowPos(AppState::Get().hwndOverlay, NULL, savedX, savedY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    
    OverlayUI::UpdateSize(); 

    ShowWindow(hwndControl, nCmdShow);
    ShowWindow(AppState::Get().hwndOverlay, SW_SHOWNA);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    InputManager::Remove();
    
    AppState::Get().SaveConfig();

    Gdiplus::GdiplusShutdown(gdiplusToken);
    if (AppState::Get().hFontUI) DeleteObject(AppState::Get().hFontUI);
    
    return 0;
}