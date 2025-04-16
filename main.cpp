// this file contains all the source code, some of it is forked from other files, some of it is custom made.
// written in c++
#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <windowsx.h>  // for SelectPen/SelectBrush if needed
#include <mmsystem.h>

int main(){
    SetProcessDPIAware(); // Ensure full-screen capture even when taskbar is hovered
    // Replace PlaySound with mciSendString to play an MP3 file
    mciSendString("open \"C:\\Users\\damian\\Downloads\\salinewinmp3.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
    mciSendString("play mp3", NULL, 0, NULL);
    srand((unsigned int)time(NULL));
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);
    // Persistent off-screen backbuffer creation
    HDC desktopDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(desktopDC);
    HBITMAP backBuffer = CreateCompatibleBitmap(desktopDC, screenX, screenY);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, backBuffer);
    // Initialize backbuffer with current desktop
    BitBlt(memDC, 0, 0, screenX, screenY, desktopDC, 0, 0, SRCCOPY);
    
    // Stage and cycle counters; now 7 stages (0–6)
    int stage = 0;
    int cycles = 0;
    int cycleCount = 0; // one full cycle = stages 0-6 then back to 0

    while(1){
        cycles++;
        if(cycles > 500){ // 500 iterations * 10ms = 5000ms = 5 sec per stage
            stage = (stage + 1) % 7;  // stages 0..6
            cycles = 0;
            if(stage == 0)
                cycleCount++; // one full cycle completed
        }

        int effect = 0;
        if(stage == 0)
            effect = rand() % 5;             // effects 0–4
        else if(stage == 1)
            effect = 5 + (rand() % 5);         // effects 5–9
        else if(stage == 2)
            effect = 10 + (rand() % 4);        // effects 10–13
        else if(stage == 3)
            effect = 14 + (rand() % 3);        // effects 14–16
        else if(stage == 4)
            effect = 17 + (rand() % 2);        // effects 17–18
        else if(stage == 5)
            effect = 19 + (rand() % 2);        // effects 19–20
        else if(stage == 6)
            effect = 21;                     // effect 21

        if(effect == 0){
            // Sinus wave distortion over backbuffer (more extreme offsets)
            for(int y = 0; y < screenY; y++){
                int offset = (int)(50 * sin((double)y / 10.0));
                if(offset > 0)
                    BitBlt(memDC, offset, y, screenX - offset, 1, memDC, 0, y, SRCCOPY);
                else
                    BitBlt(memDC, 0, y, screenX + offset, 1, memDC, -offset, y, SRCCOPY);
            }
        } else if(effect == 1){
            // Faster random color scribbling over backbuffer
            for(int i = 0; i < 2000; i++){
                int x = rand() % screenX;
                int y = rand() % screenY;
                SetPixel(memDC, x, y, RGB(rand()%256, rand()%256, rand()%256));
            }
        } else if(effect == 2){
            // More extreme random shifting effect
            int offsetX = (rand() % 41) - 20; // increased range
            int offsetY = (rand() % 41) - 20;
            StretchBlt(memDC, offsetX, offsetY, screenX, screenY, memDC, 0, 0, screenX, screenY, SRCCOPY);
        } else if(effect == 3){
            // Draw a random filled rectangle with larger random size
            RECT rect;
            rect.left = rand() % screenX;
            rect.top = rand() % screenY;
            rect.right = rect.left + (rand() % (300));
            rect.bottom = rect.top + (rand() % (300));
            HBRUSH brush = CreateSolidBrush(RGB(rand()%256, rand()%256, rand()%256));
            FillRect(memDC, &rect, brush);
            DeleteObject(brush);
        } else if(effect == 4){
            // Invert a random rectangle
            RECT invRect;
            invRect.left = rand() % screenX;
            invRect.top = rand() % screenY;
            invRect.right = invRect.left + (rand() % (screenX - invRect.left));
            invRect.bottom = invRect.top + (rand() % (screenY - invRect.top));
            InvertRect(memDC, &invRect);
        } else if(effect == 5){
            // Horizontal random offset effect; more extreme version
            int rx = rand() % screenX;
            BitBlt(memDC, rx, 5, 150, screenY, memDC, rx, 0, SRCCOPY);
        } else if(effect == 6){
            // Vertical stretching effect with larger vertical expansion
            StretchBlt(memDC, 0, -40, screenX, screenY+80, memDC, 0, 0, screenX, screenY, SRCCOPY);
        } else if(effect == 7){
            // Multi-directional extreme shifts
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, -50, 0, SRCCOPY);
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, screenX - 50, 0, SRCCOPY);
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, 0, -50, SRCCOPY);
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, 0, screenY - 50, SRCCOPY);
        } else if(effect == 8){
            // Minimal random offset effect (using extreme randomness)
            BitBlt(memDC, rand() % 2, rand() % 2, screenX, screenY, memDC, rand() % 2, rand() % 2, SRCCOPY);
        } else if(effect == 9){
            // Pixel manipulation using a DIB section for exotic visuals
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = screenX;
            bmi.bmiHeader.biHeight = -screenY; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* bits = NULL;
            HDC tempDC = CreateCompatibleDC(memDC);
            HBITMAP hbm = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
            HBITMAP oldTemp = (HBITMAP)SelectObject(tempDC, hbm);
            BitBlt(tempDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
            int* pixel = (int*)bits;
            for (int i = 0; i < screenX * screenY; i++) {
                pixel[i] ^= 0x00FF00FF;
            }
            BitBlt(memDC, 0, 0, screenX, screenY, tempDC, 0, 0, SRCCOPY);
            SelectObject(tempDC, oldTemp);
            DeleteObject(hbm);
            DeleteDC(tempDC);
        } else if(effect == 10){
            // Stage 2: Red-blue polygon drawing (no change, persists on backbuffer)
            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
            HPEN hOldPen = (HPEN)SelectObject(memDC, hPen);
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
            HBRUSH hOldBrush = (HBRUSH)SelectObject(memDC, hBrush);
            POINT vertices[3] = { {rand() % screenX, rand() % screenY}, 
                                  {rand() % screenX, rand() % screenY}, 
                                  {rand() % screenX, rand() % screenY} };
            Polygon(memDC, vertices, 3);
            SelectObject(memDC, hOldBrush);
            DeleteObject(hBrush);
            SelectObject(memDC, hOldPen);
            DeleteObject(hPen);
        } else if(effect == 11){
            // Stage 2: SRCAND effect
            BitBlt(memDC, rand()%2, rand()%2, screenX, screenY, memDC, rand()%2, rand()%2, SRCAND);
        } else if(effect == 12){
            // Stage 2: Horizontal stretching effect
            StretchBlt(memDC, -20, 0, screenX+40, screenY, memDC, 0, 0, screenX, screenY, SRCCOPY);
        } else if(effect == 13){
            // Stage 2: Elliptical NOTSRCCOPY effect
            RECT rect;
            GetWindowRect(GetDesktopWindow(), &rect);
            int w = rect.right - rect.left;
            int h = rect.bottom - rect.top;
            int x = rand() % (w - 100);
            int y = rand() % (h - 100);
            HRGN hrgn = CreateEllipticRgn(x, y, x+100, y+100);
            SelectClipRgn(memDC, hrgn);
            BitBlt(memDC, x, y, 100, 100, memDC, x, y, NOTSRCCOPY);
            SelectClipRgn(memDC, NULL);
            DeleteObject(hrgn);
        } else if(effect == 14){
            // Stage 3: Shader-like effect (based on shader1)
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = screenX;
            bmi.bmiHeader.biHeight = -screenY;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* bits = NULL;
            HDC tempDC = CreateCompatibleDC(memDC);
            HBITMAP hbm = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
            HBITMAP oldTemp = (HBITMAP)SelectObject(tempDC, hbm);
            BitBlt(tempDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
            int* pixels = (int*)bits;
            for (int i = 0; i < screenX * screenY; i++) {
                int x = i % screenX, y = i / screenX;
                pixels[i] = RGB((x ^ y) & 0xFF, (x & y) & 0xFF, (x | y) & 0xFF);
            }
            BitBlt(memDC, 0, 0, screenX, screenY, tempDC, 0, 0, SRCCOPY);
            SelectObject(tempDC, oldTemp);
            DeleteObject(hbm);
            DeleteDC(tempDC);
        } else if(effect == 15){
            // Stage 3: PlgBlt-based distortion (based on thing6)
            RECT rect;
            GetWindowRect(GetDesktopWindow(), &rect);
            POINT pts[3];
            int inc = (rand() % 701) - 350; // from -350 to 350
            pts[0].x = rect.left + inc;
            pts[0].y = rect.top - inc;
            pts[1].x = rect.right - inc;
            pts[1].y = rect.top + inc;
            pts[2].x = rect.left + inc;
            pts[2].y = rect.bottom - inc;
            HDC tempDC = CreateCompatibleDC(memDC);
            HBITMAP tempBmp = CreateCompatibleBitmap(memDC, screenX, screenY);
            SelectObject(tempDC, tempBmp);
            BitBlt(tempDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
            PlgBlt(memDC, pts, memDC, 0, 0, screenX, screenY, 0, 0, 0);
            DeleteObject(tempBmp);
            DeleteDC(tempDC);
        } else if(effect == 16){
            // Stage 3: PolyBezier effect (based on PolyBezier example)
            POINT p[4];
            p[0].x = rand() % screenX; p[0].y = rand() % screenY;
            p[1].x = rand() % screenX; p[1].y = rand() % screenY;
            p[2].x = rand() % screenX; p[2].y = rand() % screenY;
            p[3].x = rand() % screenX; p[3].y = rand() % screenY;
            HPEN hPen = CreatePen(PS_SOLID, 5, RGB(rand()%256, rand()%256, rand()%256));
            HPEN oldPen = (HPEN)SelectObject(memDC, hPen);
            PolyBezier(memDC, p, 4);
            SelectObject(memDC, oldPen);
            DeleteObject(hPen);
        } else if(effect == 17){
            // Stage 4: DIB manipulation with 24-bit bitmap (based on RGBTRIPLE example)
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = screenX;
            bmi.bmiHeader.biHeight = screenY;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 24;
            bmi.bmiHeader.biCompression = BI_RGB;
            int bufferSize = ((screenX * 3 + 3) & ~3) * screenY;
            void* pixels = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            HDC tempDC = CreateCompatibleDC(memDC);
            HBITMAP hbm = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pixels, NULL, 0);
            HBITMAP oldTemp = (HBITMAP)SelectObject(tempDC, hbm);
            BitBlt(tempDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
            unsigned char* p = (unsigned char*)pixels;
            for (int i = 0; i < bufferSize; i++) {
                p[i] = (unsigned char)(p[i] + (rand() % 50) - 25);
            }
            BitBlt(memDC, 0, 0, screenX, screenY, tempDC, 0, 0, SRCCOPY);
            SelectObject(tempDC, oldTemp);
            DeleteObject(hbm);
            DeleteDC(tempDC);
            VirtualFree(pixels, 0, MEM_RELEASE);
        } else if(effect == 18){
            // Stage 4: Extreme random scatter overlay
            for (int i = 0; i < 5000; i++){
                int x = rand() % screenX;
                int y = rand() % screenY;
                SetPixel(memDC, x, y, RGB(rand()%256, rand()%256, rand()%256));
            }
        } else if(effect == 19){
            // Stage 5: Horizontal band copy effect (based on new example)
            int x = rand() % screenX;
            BitBlt(memDC, x, 1, 10, screenY, memDC, x, 0, SRCCOPY);
        } else if(effect == 20){
            // Stage 5: PATINVERT effect over entire backbuffer
            PatBlt(memDC, 0, 0, screenX, screenY, PATINVERT);
        } else if(effect == 21){
            // Stage 6: New 24-bit DIB effect with XOR manipulation
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = screenX;
            bmi.bmiHeader.biHeight = screenY;  // bottom-up
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 24;
            bmi.bmiHeader.biCompression = BI_RGB;
            int bufSize = ((screenX * 3 + 3) & ~3) * screenY;
            void* pixels = VirtualAlloc(0, bufSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            HDC tempDC = CreateCompatibleDC(memDC);
            HBITMAP hbm = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &pixels, NULL, 0);
            HBITMAP oldTemp = (HBITMAP)SelectObject(tempDC, hbm);
            BitBlt(tempDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
            unsigned char* pByte = (unsigned char*)pixels;
            for (int i = 0; i < bufSize; i++) {
                pByte[i] = (unsigned char)(pByte[i] ^ (rand() % 256));
            }
            BitBlt(memDC, 0, 0, screenX, screenY, tempDC, 0, 0, SRCCOPY);
            SelectObject(tempDC, oldTemp);
            DeleteObject(hbm);
            DeleteDC(tempDC);
            VirtualFree(pixels, 0, MEM_RELEASE);
        }
        // Blit the persistent backbuffer to the desktop so visuals persist
        BitBlt(desktopDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
        Sleep(10); // Faster frame rate: 10ms per iteration
    }
    
    // Stop and close the MP3 playback
    mciSendString("stop mp3", NULL, 0, NULL);
    mciSendString("close mp3", NULL, 0, NULL);
    return 0;
}

// Compile using:
// gcc Untitled-2.c -o Untitled-2.exe -lgdi32 -lwinmm
