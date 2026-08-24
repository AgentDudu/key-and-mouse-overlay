#pragma once
#include <windows.h>

class ScopedDC {
    HWND hwnd; HDC hdc;
public:
    ScopedDC(HWND h) : hwnd(h), hdc(GetDC(h)) {}
    ~ScopedDC() { if (hdc) ReleaseDC(hwnd, hdc); }
    operator HDC() const { return hdc; }
};

class ScopedMemDC {
    HDC hdc;
public:
    ScopedMemDC(HDC hdcScreen) : hdc(CreateCompatibleDC(hdcScreen)) {}
    ~ScopedMemDC() { if (hdc) DeleteDC(hdc); }
    operator HDC() const { return hdc; }
};

template <typename T>
class ScopedGDIObj {
    T obj;
public:
    ScopedGDIObj(T o) : obj(o) {}
    ~ScopedGDIObj() { if (obj) DeleteObject(obj); }
    operator T() const { return obj; }
};

using ScopedBitmap = ScopedGDIObj<HBITMAP>;
using ScopedBrush = ScopedGDIObj<HBRUSH>;

class ScopedSelect {
    HDC hdc; HGDIOBJ oldObj;
public:
    ScopedSelect(HDC h, HGDIOBJ newObj) : hdc(h) { oldObj = SelectObject(hdc, newObj); }
    ~ScopedSelect() { if (oldObj) SelectObject(hdc, oldObj); }
};

class ScopedPaint {
    HWND hwnd; PAINTSTRUCT ps; HDC hdc;
public:
    ScopedPaint(HWND h) : hwnd(h) { hdc = BeginPaint(hwnd, &ps); }
    ~ScopedPaint() { EndPaint(hwnd, &ps); }
    operator HDC() const { return hdc; }
};

namespace Win32Utils {
    inline void EnsureWindowOnScreen(int& x, int& y, int width, int height) {
        RECT rc = { x, y, x + width, y + height };
        HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
        if (hMonitor) {
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfoW(hMonitor, &mi)) {
                if (x < mi.rcWork.left) x = mi.rcWork.left;
                if (x + width > mi.rcWork.right) x = mi.rcWork.right - width;
                
                if (y < mi.rcWork.top) y = mi.rcWork.top;
                if (y + height > mi.rcWork.bottom) y = mi.rcWork.bottom - height;
            }
        }
    }
}