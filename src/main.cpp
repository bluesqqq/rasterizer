#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdint.h>
#include "FrameBuffer.h"
#include <cmath>

static FrameBuffer g_framebuffer;
static HWND g_hwnd;

void update() {

}

float i = 0;

void render() {
    if (!g_framebuffer.pixels) return;

    g_framebuffer.clear(0xFF000000); // ARGB

    fillRect(g_framebuffer, 50, 200, 200, 300, 0xFFFF0000);
    drawLine(g_framebuffer, 500, 500, 500 + cos(i) * 300, 500 + sin(i) * 300, 0xFFFFFFFF);
    g_framebuffer.present(g_hwnd);

    i += 0.01;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Rasterizer";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Rasterizer",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL) return 0;

    g_hwnd = hwnd;

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    bool running = true;

    MSG msg = { };
    while(running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        update();

        render();
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            if (width > 0 && height > 0) {
                g_framebuffer.resize(width, height);
            }
            return 0;
        }

        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}