#pragma once

#include <windows.h>
#include <stdint.h>

struct FrameBuffer {
    using pointer  = uint32_t*;
    using iterator = uint32_t*;
    using const_iterator = const uint32_t*;

    int width = 0, height = 0;
    pointer pixels = nullptr;
    BITMAPINFO bitmapInfo;

    // Constructors + Destructors

    FrameBuffer() = default;

    FrameBuffer(int w, int h);

    ~FrameBuffer();

    // Iterators

    iterator begin() noexcept;
    const_iterator begin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;

    iterator at(int x, int y);
    const_iterator at(int x, int y) const;

    // Size

    void resize(int w, int h);

    int size() const;
    bool empty() const;

    // Drawing

    void clear(uint32_t color);

    void setPix(int x, int y, uint32_t color);
    void setPix(iterator p, uint32_t color);

    uint32_t getPix(int x, int y) const;
    uint32_t getPix(const_iterator p) const;

    // Windows

    void present(HWND hwnd);
};

void fillRect(FrameBuffer& fb, int x, int y, int w, int h, uint32_t color);

void drawLine(FrameBuffer &fb, int x0, int y0, int x1, int y1, uint32_t color);