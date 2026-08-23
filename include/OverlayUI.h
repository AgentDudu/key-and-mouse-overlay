#pragma once
#include <windows.h>
#include <gdiplus.h>
#include "AppState.h"
#include "Win32Utils.h" 
#include "CoreLogic.h"
#include "resource.h" 

#define WM_REDRAW_OVERLAY (WM_USER + 1)

class OverlayUI {
private:
    static const wchar_t* GetKeyString(int key, wchar_t* buf) {
        if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z')) { buf[0] = (wchar_t)key; buf[1] = L'\0'; return buf; }
        switch(key) {
            case VK_UP: return L"↑"; case VK_DOWN: return L"↓"; case VK_LEFT: return L"←"; case VK_RIGHT: return L"→";
            case VK_NUMPAD0: return L"0"; case VK_NUMPAD1: return L"1"; case VK_NUMPAD2: return L"2"; case VK_NUMPAD3: return L"3";
            case VK_NUMPAD4: return L"4"; case VK_NUMPAD5: return L"5"; case VK_NUMPAD6: return L"6"; case VK_NUMPAD7: return L"7";
            case VK_NUMPAD8: return L"8"; case VK_NUMPAD9: return L"9"; case VK_DIVIDE: return L"/"; case VK_MULTIPLY: return L"*";
            case VK_SUBTRACT: return L"-"; case VK_ADD: return L"+"; case VK_DECIMAL: return L"."; case VK_RETURN: return L"EN";
        }
        return L"?";
    }

    static void DrawRoundRect(Gdiplus::Graphics& g, int x, int y, int w, int h, int r, Gdiplus::Brush* fill, Gdiplus::Pen* outline) {
        Gdiplus::GraphicsPath path; int d = r * 2;
        path.AddArc(x, y, d, d, 180, 90); path.AddArc(x + w - d, y, d, d, 270, 90);
        path.AddArc(x + w - d, y + h - d, d, d, 0, 90); path.AddArc(x, y + h - d, d, d, 90, 90);
        path.CloseFigure();
        if (fill) g.FillPath(fill, &path); if (outline) g.DrawPath(outline, &path);
    }

    static void DrawButton(Gdiplus::Graphics& g, int x, int y, int w, int h, const wchar_t* text, bool pressed) {
        AppState& state = AppState::Get();
        int sx = (int)(x * state.uiScale), sy = (int)(y * state.uiScale), sw = (int)(w * state.uiScale), sh = (int)(h * state.uiScale);
        
        int bAlpha = (state.baseAlpha * 255) / 100, hAlpha = (state.highlightAlpha * 255) / 100, oAlpha = (state.outlineAlpha * 255) / 100;
        Gdiplus::Color bgColor = pressed ? Gdiplus::Color(hAlpha, GetRValue(state.activeBg), GetGValue(state.activeBg), GetBValue(state.activeBg)) 
                                         : Gdiplus::Color(bAlpha, GetRValue(state.inactiveBg), GetGValue(state.inactiveBg), GetBValue(state.inactiveBg));
        Gdiplus::SolidBrush brush(bgColor);
        Gdiplus::Pen outlinePen(Gdiplus::Color(oAlpha, GetRValue(state.outlineColor), GetGValue(state.outlineColor), GetBValue(state.outlineColor)), 1.5f);

        g.FillRectangle(&brush, sx, sy, sw, sh);
        if (oAlpha > 0) g.DrawRectangle(&outlinePen, sx, sy, sw, sh);

        Gdiplus::Color txtColor = pressed ? Gdiplus::Color(255, GetRValue(state.activeText), GetGValue(state.activeText), GetBValue(state.activeText)) 
                                          : Gdiplus::Color(255, GetRValue(state.inactiveText), GetGValue(state.inactiveText), GetBValue(state.inactiveText));
        Gdiplus::SolidBrush textBrush(txtColor); Gdiplus::FontFamily fontFamily(L"Segoe UI");
        Gdiplus::Font font(&fontFamily, (float)(18 * state.uiScale), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter); format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(text, -1, &font, Gdiplus::RectF((float)sx, (float)sy, (float)sw, (float)sh), &format, &textBrush);
    }

    static void DrawMouseUI(Gdiplus::Graphics& g, int x, int y) {
        AppState& state = AppState::Get();
        int sx = (int)(x * state.uiScale), sy = (int)(y * state.uiScale);
        int bw = (int)(35 * state.uiScale), bh = (int)(45 * state.uiScale), gap = (int)(5 * state.uiScale); 
        int bodyH = (int)(55 * state.uiScale), corner = (int)(6 * state.uiScale); 
        int bAlpha = (state.baseAlpha * 255) / 100, hAlpha = (state.highlightAlpha * 255) / 100, oAlpha = (state.outlineAlpha * 255) / 100;

        auto makeColor = [&](bool active) {
            return active ? Gdiplus::Color(hAlpha, GetRValue(AppState::Get().activeBg), GetGValue(AppState::Get().activeBg), GetBValue(AppState::Get().activeBg))
                          : Gdiplus::Color(bAlpha, GetRValue(AppState::Get().inactiveBg), GetGValue(AppState::Get().inactiveBg), GetBValue(AppState::Get().inactiveBg));
        };

        Gdiplus::SolidBrush lmb(makeColor(state.lmb)), rmb(makeColor(state.rmb)), body(makeColor(false)), m4(makeColor(state.mb4)), m5(makeColor(state.mb5));
        Gdiplus::SolidBrush wheel(state.mmb || state.isScrolling ? makeColor(true) : Gdiplus::Color(bAlpha, GetRValue(state.inactiveText), GetGValue(state.inactiveText), GetBValue(state.inactiveText)));
        Gdiplus::Pen outPen(Gdiplus::Color(oAlpha, GetRValue(state.outlineColor), GetGValue(state.outlineColor), GetBValue(state.outlineColor)), 1.5f);
        Gdiplus::Pen* pOut = (oAlpha > 0) ? &outPen : nullptr;

        int sideW = (int)(8 * state.uiScale), sideH = (int)(22 * state.uiScale), sideX = sx - sideW - (int)(2 * state.uiScale);
        if (state.showMB5) DrawRoundRect(g, sideX, sy + bh + (int)(4 * state.uiScale), sideW, sideH, (int)(2*state.uiScale), &m5, pOut);
        if (state.showMB4) DrawRoundRect(g, sideX, sy + bh + (int)(29 * state.uiScale), sideW, sideH, (int)(2*state.uiScale), &m4, pOut);

        DrawRoundRect(g, sx, sy, bw, bh, corner, &lmb, pOut);
        DrawRoundRect(g, sx + bw + gap, sy, bw, bh, corner, &rmb, pOut);
        DrawRoundRect(g, sx, sy + bh + (int)(3 * state.uiScale), bw * 2 + gap, bodyH, corner * 2, &body, pOut);
        DrawRoundRect(g, sx + bw - (int)(1 * state.uiScale), sy + (int)(15 * state.uiScale), gap + (int)(2 * state.uiScale), (int)(18 * state.uiScale), (int)(2*state.uiScale), &wheel, pOut);

        Gdiplus::SolidBrush textL(Gdiplus::Color(255, GetRValue(state.lmb ? state.activeText : state.inactiveText), GetGValue(state.lmb ? state.activeText : state.inactiveText), GetBValue(state.lmb ? state.activeText : state.inactiveText)));
        Gdiplus::SolidBrush textR(Gdiplus::Color(255, GetRValue(state.rmb ? state.activeText : state.inactiveText), GetGValue(state.rmb ? state.activeText : state.inactiveText), GetBValue(state.rmb ? state.activeText : state.inactiveText)));
        Gdiplus::FontFamily fontFamily(L"Segoe UI"); Gdiplus::Font font(&fontFamily, (float)(16 * state.uiScale), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter); format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(L"L", -1, &font, Gdiplus::RectF((float)sx, (float)sy, (float)bw, (float)bh), &format, &textL);
        g.DrawString(L"R", -1, &font, Gdiplus::RectF((float)(sx + bw + gap), (float)sy, (float)bw, (float)bh), &format, &textR);

        if (state.showCPS) {
            ULONGLONG now = GetTickCount64();
            while (!state.clickTimes.empty() && now - state.clickTimes.front() > 1000) state.clickTimes.erase(state.clickTimes.begin());
            wchar_t cpsBuf[16]; wsprintfW(cpsBuf, L"%d CPS", (int)state.clickTimes.size());
            Gdiplus::Font cpsFont(&fontFamily, (float)(12 * state.uiScale), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            Gdiplus::SolidBrush cpsBrush(Gdiplus::Color(255, GetRValue(state.inactiveText), GetGValue(state.inactiveText), GetBValue(state.inactiveText)));
            Gdiplus::RectF bodyRect((float)sx, (float)(sy + bh + (int)(3 * state.uiScale)), (float)(bw * 2 + gap), (float)bodyH);
            g.DrawString(cpsBuf, -1, &cpsFont, bodyRect, &format, &cpsBrush);
        }
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();
        
        if (uMsg == WM_TIMER && wParam == 2) {
            ULONGLONG now = GetTickCount64(); bool changed = false;
            while (!state.clickTimes.empty() && now - state.clickTimes.front() > 1000) { state.clickTimes.erase(state.clickTimes.begin()); changed = true; }
            if (changed) PostMessage(hwnd, WM_REDRAW_OVERLAY, 0, 0);
            if (state.clickTimes.empty()) KillTimer(hwnd, 2); 
            return 0;
        }

        if (uMsg == WM_TIMER && wParam == 1) { state.isScrolling = false; KillTimer(hwnd, 1); PostMessage(hwnd, WM_REDRAW_OVERLAY, 0, 0); return 0; }

        if (uMsg == WM_REDRAW_OVERLAY) {
            int width = (int)(state.dynamicWidth * state.uiScale), height = (int)(state.dynamicHeight * state.uiScale);
            ScopedDC hdcScreen(NULL); ScopedMemDC hdcMem(hdcScreen);
            
            BITMAPINFO bmi = {0}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth = width; bmi.bmiHeader.biHeight = -height; bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
            void* pBits = NULL; ScopedBitmap hBitmap(CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0)); 
            
            {
                ScopedSelect autoBmp(hdcMem, hBitmap); 
                Gdiplus::Graphics g(hdcMem); g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
                if (!state.isLocked) { Gdiplus::SolidBrush bg(Gdiplus::Color(100, 0, 0, 0)); g.FillRectangle(&bg, 0, 0, width, height); }

                LayoutInfo li = LayoutEngine::ComputeLayout(state.showExtraKey, state.showSpace, state.showShift, state.showCtrl);
                
                for (int i = 0; i < 256; i++) {
                    if (!state.showExtraKey[i]) continue;
                    int bc, br; float ox;
                    if (LayoutEngine::GetGridPos(i, bc, ox, br)) {
                        int x = (int)((li.colMap[bc] + ox) * 50) + 15, y = (int)(li.rowMap[br] * 50) + 10;
                        wchar_t txt[4]; DrawButton(g, x, y, 45, 45, GetKeyString(i, txt), state.keys[i]);
                    }
                }
                if (state.showSpace) DrawButton(g, 15, li.spaceY, 145, 25, L"SPACE", state.keys[VK_SPACE]);
                if (state.showShift || state.showCtrl) {
                    int curX = 15;
                    if (state.showShift) { DrawButton(g, curX, li.shiftCtrlY, 70, 25, L"SHIFT", state.keys[VK_LSHIFT] || state.keys[VK_SHIFT]); curX += 75; }
                    if (state.showCtrl) DrawButton(g, curX, li.shiftCtrlY, 70, 25, L"CTRL", state.keys[VK_LCONTROL] || state.keys[VK_CONTROL]);
                }
                DrawMouseUI(g, li.mouseX, 10);
                
                POINT ptSrc = {0, 0}; SIZE size = { width, height }; BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                RECT rc; GetWindowRect(hwnd, &rc); POINT ptPos = { rc.left, rc.top };
                UpdateLayeredWindow(hwnd, hdcScreen, &ptPos, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
            } 
            return 0;
        }
        else if (uMsg == WM_NCHITTEST && !state.isLocked) return HTCAPTION;
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static HWND Create(HINSTANCE hInstance) {
        WNDCLASSW wc = { }; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = L"OverlayClass";
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); RegisterClassW(&wc);
        HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, L"OverlayClass", L"Overlay", WS_POPUP, 400, 400, AppState::Get().dynamicWidth, AppState::Get().dynamicHeight, NULL, NULL, hInstance, NULL);
        return hwnd;
    }
    static void UpdateSize() {
        AppState& state = AppState::Get(); 
        
        LayoutInfo li = LayoutEngine::ComputeLayout(state.showExtraKey, state.showSpace, state.showShift, state.showCtrl);
        
        state.dynamicHeight = li.lowestY; state.dynamicWidth = li.mouseRightEdge; 
        SetWindowPos(state.hwndOverlay, NULL, 0, 0, (int)(state.dynamicWidth * state.uiScale), (int)(state.dynamicHeight * state.uiScale), SWP_NOMOVE | SWP_NOZORDER);
        PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0);
    }
};