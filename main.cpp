#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <windowsx.h> 
#include <mmsystem.h>

int main(){
    SetProcessDPIAware(); 
    mciSendString("PATH_HERE (USE \\)" type mpegvideo alias mp3", NULL, 0, NULL);
    mciSendString("play mp3", NULL, 0, NULL);
    srand((unsigned int)time(NULL));
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);
    HDC desktopDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(desktopDC);
    HBITMAP backBuffer = CreateCompatibleBitmap(desktopDC, screenX, screenY);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, backBuffer);
    BitBlt(memDC, 0, 0, screenX, screenY, desktopDC, 0, 0, SRCCOPY);
    
    int stage = 0;
    int cycles = 0;
    int cycleCount = 0;

    while(1){
        cycles++;
        if(cycles > 500){
            stage = (stage + 1) % 7;
            cycles = 0;
            if(stage == 0)
                cycleCount++;
        }

        int effect = 0;
        if(stage == 0)
            effect = rand() % 5;
        else if(stage == 1)
            effect = 5 + (rand() % 5);
        else if(stage == 2)
            effect = 10 + (rand() % 4);
        else if(stage == 3)
            effect = 14 + (rand() % 3);
        else if(stage == 4)
            effect = 17 + (rand() % 2);
        else if(stage == 5)
            effect = 19 + (rand() % 2);
        else if(stage == 6)
            effect = 21;

        if(effect == 0){
            for(int y = 0; y < screenY; y++){
                int offset = (int)(50 * sin((double)y / 10.0));
                if(offset > 0)
                    BitBlt(memDC, offset, y, screenX - offset, 1, memDC, 0, y, SRCCOPY);
                else
                    BitBlt(memDC, 0, y, screenX + offset, 1, memDC, -offset, y, SRCCOPY);
            }
        } else if(effect == 1){
            for(int i = 0; i < 2000; i++){
                int x = rand() % screenX;
                int y = rand() % screenY;
                SetPixel(memDC, x, y, RGB(rand()%256, rand()%256, rand()%256));
            }
        } else if(effect == 2){
            int offsetX = (rand() % 41) - 20;
            int offsetY = (rand() % 41) - 20;
            StretchBlt(memDC, offsetX, offsetY, screenX, screenY, memDC, 0, 0, screenX, screenY, SRCCOPY);
        } else if(effect == 3){
            RECT rect;
            rect.left = rand() % screenX;
            rect.top = rand() % screenY;
            rect.right = rect.left + (rand() % (300));
            rect.bottom = rect.top + (rand() % (300));
            HBRUSH brush = CreateSolidBrush(RGB(rand()%256, rand()%256, rand()%256));
            FillRect(memDC, &rect, brush);
            DeleteObject(brush);
        } else if(effect == 4){
            RECT invRect;
            invRect.left = rand() % screenX;
            invRect.top = rand() % screenY;
            invRect.right = invRect.left + (rand() % (screenX - invRect.left));
            invRect.bottom = invRect.top + (rand() % (screenY - invRect.top));
            InvertRect(memDC, &invRect);
        } else if(effect == 5){
            int rx = rand() % screenX;
            BitBlt(memDC, rx, 5, 150, screenY, memDC, rx, 0, SRCCOPY);
        } else if(effect == 6){
            StretchBlt(memDC, 0, -40, screenX, screenY+80, memDC, 0, 0, screenX, screenY, SRCCOPY);
        } else if(effect == 7){
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, -50, 0, SRCCOPY);
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, screenX - 50, 0, SRCCOPY);
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, 0, -50, SRCCOPY);
            BitBlt(memDC, 0, 0, screenX, screenY, memDC, 0, screenY - 50, SRCCOPY);
        } else if(effect == 8){
            BitBlt(memDC, rand() % 2, rand() % 2, screenX, screenY, memDC, rand() % 2, rand() % 2, SRCCOPY);
        } else if(effect == 9){
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
            int* pixel = (int*)bits;
            for (int i = 0; i < screenX * screenY; i++) {
                pixel[i] ^= 0x00FF00FF;
            }
            BitBlt(memDC, 0, 0, screenX, screenY, tempDC, 0, 0, SRCCOPY);
            SelectObject(tempDC, oldTemp);
            DeleteObject(hbm);
            DeleteDC(tempDC);
        } else if(effect == 10){
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
            BitBlt(memDC, rand()%2, rand()%2, screenX, screenY, memDC, rand()%2, rand()%2, SRCAND);
        } else if(effect == 12){
            StretchBlt(memDC, -20, 0, screenX+40, screenY, memDC, 0, 0, screenX, screenY, SRCCOPY);
        } else if(effect == 13){
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
        }
        BitBlt(desktopDC, 0, 0, screenX, screenY, memDC, 0, 0, SRCCOPY);
        Sleep(10);
    }
}
