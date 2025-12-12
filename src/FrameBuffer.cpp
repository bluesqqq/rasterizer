#include "FrameBuffer.h"
#include <utility> 

FrameBuffer::FrameBuffer(int w, int h) { resize(w, h); }

FrameBuffer::~FrameBuffer() {
    if (pixels) VirtualFree(pixels, 0, MEM_RELEASE);
}

FrameBuffer::iterator FrameBuffer::begin() noexcept { return pixels; }

FrameBuffer::const_iterator FrameBuffer::begin() const noexcept { return pixels; }

FrameBuffer::iterator FrameBuffer::end() noexcept { return pixels + width * height; }

FrameBuffer::const_iterator FrameBuffer::end() const noexcept { return pixels + width * height; }

FrameBuffer::iterator FrameBuffer::at(int x, int y) {
    if ((unsigned)x >= (unsigned)width ||
        (unsigned)y >= (unsigned)height)
        return end();

    return pixels + y * width + x;
}

FrameBuffer::const_iterator FrameBuffer::at(int x, int y) const {
    if ((unsigned)x >= (unsigned)width ||
        (unsigned)y >= (unsigned)height)
        return end();

    return pixels + y * width + x;
}

void FrameBuffer::resize(int w, int h) {
    width = w;
    height = h;

    if (pixels) VirtualFree(pixels, 0, MEM_RELEASE);

    if (width <= 0 || height <= 0) {
        pixels = nullptr;
        return;
    }

    pixels = (uint32_t*)VirtualAlloc(
        nullptr,
        width * height * sizeof(uint32_t),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height; // top-down
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
}

int FrameBuffer::size() const { return width * height; }

bool FrameBuffer::empty() const { return pixels == nullptr || width == 0 || height == 0; }

void FrameBuffer::clear(uint32_t color) {
    iterator p = begin();
    iterator e = end();
    while (p < e) {
        *p++ = color;
    }
}

void FrameBuffer::setPix(int x, int y, uint32_t color) {
    if ((unsigned)x < (unsigned)width && (unsigned)y < unsigned(height))
        pixels[y * width + x] = color;
}

void FrameBuffer::setPix(iterator p, uint32_t color) {
    if (p >= begin() && p < end())
        *p = color;
}

uint32_t FrameBuffer::getPix(int x, int y) const {
    if ((unsigned)x < (unsigned)width && (unsigned)y < unsigned(height))
        return pixels[y * width + x];

    return 0x00000000;
}

uint32_t FrameBuffer::getPix(const_iterator p) const {
    if (p >= begin() && p < end())
        return *p;

    return 0x00000000;
}

void FrameBuffer::present(HWND hwnd) {
    if (!pixels) return;
    
    HDC hdc = GetDC(hwnd);

    StretchDIBits(hdc, 
        0, 0, width, height,
        0, 0, width, height,
        pixels,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );

    ReleaseDC(hwnd, hdc);
}

void fillRect(FrameBuffer &fb, int x, int y, int w, int h, uint32_t color) {
    for (int _x = 0; _x < w; ++_x)
        for (int _y = 0; _y < h; ++_y)
            fb.setPix(x + _x, y + _y, color);
}

void fillBottomFlatTriangle(FrameBuffer &fb, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float invslope0 = (float)(x1 - x0) / (float)(y1 - y0);
    float invslope1 = (float)(x2 - x0) / (float)(y2 - y0);

    float curx0 = x0;
    float curx1 = x0;

    for (int scanlineY = y0; scanlineY <= y1; scanlineY++) {
        drawScanline(fb, (int)curx0, scanlineY, (int)curx1, color);
        curx0 += invslope0;
        curx1 += invslope1;
    }
}

void fillTopFlatTriangle(FrameBuffer &fb, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float invslope0 = (float)(x2 - x0) / (float)(y2 - y0);
    float invslope1 = (float)(x2 - x1) / (float)(y2 - y1);

    float curx0 = x2;
    float curx1 = x2;

    for (int scanlineY = y2; scanlineY > y0; scanlineY--) {
        drawScanline(fb, (int)curx0, scanlineY, (int)curx1, color);
        curx0 -= invslope0;
        curx1 -= invslope1;
    }
}

inline void sortByY(int& x0, int& y0, int& x1, int& y1, int& x2, int& y2) {
    if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
    if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); }
    if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
}

void fillTriangle(FrameBuffer &fb, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // https://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html

    sortByY(x0, y0, x1, y1, x2, y2);

    if (y1 == y2) { 
        fillBottomFlatTriangle(fb, x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        fillTopFlatTriangle(fb, x0, y0, x1, y1, x2, y2, color);
    } else { // split into topflat and bottom flat
        int x3 = (int)(x0 + (float)(y1 - y0)* (x2 - x0) / (y2 - y0));
        int y3 = y1;

        fillBottomFlatTriangle(fb, x0, y0, x1, y1, x3, y3, color);
        fillTopFlatTriangle(fb, x1, y1, x3, y3, x2, y2, color);
    }
}

void drawLine(FrameBuffer &fb, int x0, int y0, int x1, int y1, uint32_t color) {
    bool steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    int dir = (dy < 0) ? -1 : 1;
    dy *= dir;

    int y = y0;
    int p = 2 * dy - dx;

    for (int x = x0; x <= x1; ++x) {
        if (steep)
            fb.setPix(y, x, color);
        else
            fb.setPix(x, y, color);

        if (p >= 0) {
            y += dir;
            p -= 2 * dx;
        }
        p += 2 * dy;
    }
}

void drawTriangle(FrameBuffer &fb, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    drawLine(fb, x0, y0, x1, y1, color);
    drawLine(fb, x1, y1, x2, y2, color);
    drawLine(fb, x2, y2, x0, y0, color);
}

void drawScanline(FrameBuffer &fb, int x0, int y, int x1, uint32_t color) {
    if (y < 0 || y >= fb.height) return;

    if (x0 > x1) std::swap(x0, x1); // left to right

    if (x1 < 0 || x0 >= fb.width) return;

    x0 = max(x0, 0);
    x1 = min(x1, fb.width - 1);

    int length = x1 - x0 + 1;
    if (length <= 0) return;

    uint32_t* p = fb.pixels + y * fb.width + x0;

    while (length--)
        *p++ = color;
}

uint32_t argb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}