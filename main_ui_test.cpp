#define UNICODE
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
ULONG_PTR gdiplusToken;

const int BTN_WIDTH = 40;
const int BTN_HEIGHT = 30;

RECT closeBtn, maxBtn, minBtn;
bool hoverClose = false, hoverMax = false, hoverMin = false;
bool isMaximized = false;
RECT prevRect = { 0 };

void DrawCleanUI(HWND hwnd, HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right;
    int height = clientRect.bottom;

    // button positions
    closeBtn = { width - 40, 0, width, BTN_HEIGHT };
    maxBtn   = { width - 80, 0, width - 40, BTN_HEIGHT };
    minBtn   = { width - 120, 0, width - 80, BTN_HEIGHT };

    // creating offscreen buffer
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

    Graphics g(memDC);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush bg(Color(255, 250, 250, 250));
    g.FillRectangle(&bg, 0, 0, width, height);

    FontFamily fontFam(L"Segoe UI");
    Font font(&fontFam, 20);
    SolidBrush text(Color(255, 50, 50, 50));
    g.DrawString(L"Hello from Clean UI", -1, &font, PointF(180, 120), &text);

    SolidBrush btnCloseBrush(hoverClose ? Color(255, 230, 80, 80) : Color(255, 200, 50, 50));
    SolidBrush btnMinBrush(hoverMin ? Color(255, 180, 180, 180) : Color(255, 160, 160, 160));
    SolidBrush btnMaxBrush(hoverMax ? Color(255, 180, 180, 255) : Color(255, 140, 140, 255));

    g.FillRectangle(&btnMinBrush, minBtn.left, minBtn.top, BTN_WIDTH, BTN_HEIGHT);
    g.FillRectangle(&btnMaxBrush, maxBtn.left, maxBtn.top, BTN_WIDTH, BTN_HEIGHT);
    g.FillRectangle(&btnCloseBrush, closeBtn.left, closeBtn.top, BTN_WIDTH, BTN_HEIGHT);

    Font btnFont(&fontFam, 16);
    SolidBrush white(Color(255, 255, 255, 255));
    g.DrawString(L"―", -1, &btnFont, PointF(minBtn.left + 12, minBtn.top + 5), &white);
    g.DrawString(L"▭", -1, &btnFont, PointF(maxBtn.left + 10, maxBtn.top + 5), &white);
    g.DrawString(L"X",  -1, &btnFont, PointF(closeBtn.left + 12, closeBtn.top + 5), &white);

    // copy from memory DC to real DC
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    GdiplusStartupInput gsi;
    GdiplusStartup(&gdiplusToken, &gsi, nullptr);

    const wchar_t CLASS_NAME[] = L"CleanUI";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Minimal Clean UI",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawCleanUI(hwnd, hdc);
            EndPaint(hwnd, &ps);
        } return 0;

        case WM_SIZE: {
            InvalidateRect(hwnd, nullptr, FALSE);
        } return 0;

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            bool h1 = PtInRect(&closeBtn, POINT{ x, y });
            bool h2 = PtInRect(&maxBtn, POINT{ x, y });
            bool h3 = PtInRect(&minBtn, POINT{ x, y });

            if (h1 != hoverClose || h2 != hoverMax || h3 != hoverMin) {
                hoverClose = h1;
                hoverMax = h2;
                hoverMin = h3;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        } return 0;

        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            POINT pt = { x, y };

            if (PtInRect(&closeBtn, pt)) {
                PostQuitMessage(0);
            } else if (PtInRect(&minBtn, pt)) {
                ShowWindow(hwnd, SW_MINIMIZE);
            } else if (PtInRect(&maxBtn, pt)) {
                if (!isMaximized) {
                    GetWindowRect(hwnd, &prevRect);
                    MONITORINFO mi = { sizeof(mi) };
                    GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
                    SetWindowPos(hwnd, nullptr,
                        mi.rcWork.left, mi.rcWork.top,
                        mi.rcWork.right - mi.rcWork.left,
                        mi.rcWork.bottom - mi.rcWork.top,
                        SWP_NOZORDER | SWP_FRAMECHANGED);
                    isMaximized = true;
                } else {
                    SetWindowPos(hwnd, nullptr,
                        prevRect.left, prevRect.top,
                        prevRect.right - prevRect.left,
                        prevRect.bottom - prevRect.top,
                        SWP_NOZORDER | SWP_FRAMECHANGED);
                    isMaximized = false;
                }
            } else if (y < BTN_HEIGHT) {
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        } return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
