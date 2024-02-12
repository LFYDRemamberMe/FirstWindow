#include "BaseWindow.h"
#include "Define.cpp"

#pragma once

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class MainWindow : public BaseWindow<MainWindow>
{
    ID2D1Factory* pFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrush;
    D2D1_ELLIPSE            ellipse;

    void    CalculateLayout();
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

public:

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
    {
    }

    PCWSTR  ClassName() const { return L"Calculator"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Recalculate drawing layout when the size of the window changes.
HINSTANCE hInst;

void MainWindow::CalculateLayout()
{
    if (pRenderTarget != NULL)
    {
        D2D1_SIZE_F size = pRenderTarget->GetSize();
        const float x = size.width / 2;
        const float y = size.height / 2;
        const float radius = min(x, y);
        ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
    }
}

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

            if (SUCCEEDED(hr))
            {
                CalculateLayout();
            }
        }
    }
    return hr;
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        
        RECT rcA;
        GetClientRect(m_hwnd, &rcA);

        D2D1_SIZE_U sizeA = D2D1::SizeU(rcA.right, rcA.bottom);

        BeginPaint(m_hwnd, &ps);
        HDC hdc = BeginPaint(m_hwnd, &ps);

        pRenderTarget->BeginDraw();

        // Create a linear gradient brush for the background
        ID2D1GradientStopCollection* pGradientStops = NULL;
        D2D1_GRADIENT_STOP gradientStops[2];
        gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::DeepPink);
        gradientStops[0].position = -2.7f;
        gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Pink);
        gradientStops[1].position = 1.9f;
        pRenderTarget->CreateGradientStopCollection(
            gradientStops,
            2,
            D2D1_GAMMA_2_2,
            D2D1_EXTEND_MODE_CLAMP,
            &pGradientStops);

        ID2D1LinearGradientBrush* pLinearGradientBrush = NULL;
        pRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(rcA.left), D2D1::Point2F(rcA.right)), pGradientStops, &pLinearGradientBrush);
        // Draw background with gradient
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        pRenderTarget->FillRectangle(
            D2D1::RectF(0, 0, rcA.right, rcA.bottom), // adjust to your window size
            pLinearGradientBrush);

        // Release resources
        SafeRelease(&pGradientStops);
        SafeRelease(&pLinearGradientBrush);

        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}


void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);
        CalculateLayout();
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    MainWindow win;
    hInst = hInstance;

    if (!win.Create(L"Калькулятор", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        static HWND hButton, hButton1, hButton2, hButton3, hButton4, hButton5, hButton6, hButton7, hButton8,
            hButton9, hButton10, hButton11, hButton12, hButton13, hButton14, hButton15, hButton16, hEdit, hListBox;
        PAINTSTRUCT ps;
        HDC hdc;
        char buff[100];
        int i = 0;
        int count = 0, a = 0, b = 0;

        HFONT hFont = CreateFont(
            /* Выберите параметры шрифта, такие как: */
            20,              // высота шрифта
            0,               // ширина символа (0 для автоматического выбора)
            0,               // угол наклона
            0,               // угол поворота
            FW_NORMAL,       // толщина шрифта (FW_BOLD для жирного текста)
            FALSE,           // курсив
            FALSE,           // подчеркнутый
            FALSE,           // зачеркнутый
            DEFAULT_CHARSET, // набор символов
            OUT_OUTLINE_PRECIS, // точность вывода
            CLIP_DEFAULT_PRECIS,
            PROOF_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            L"Arial"         // имя шрифта
        );

    switch (uMsg)
    {

    case WM_CREATE:
    {
        hEdit = CreateWindow(L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 10, 5, 515, 60, m_hwnd, (HMENU)ID_EDIT, hInst, 0);
        hButton = CreateWindow(L"Button", L"0", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 250, 430, 50, m_hwnd, (HMENU)ID_BUTTON, hInst, 0);
        hButton1 = CreateWindow(L"Button", L"1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 195, 140, 50, m_hwnd, (HMENU)ID_BUTTON_1, hInst, 0);
        hButton2 = CreateWindow(L"Button", L"2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 155, 195, 140, 50, m_hwnd, (HMENU)ID_BUTTON_2, hInst, 0);
        hButton3 = CreateWindow(L"Button", L"3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 195, 140, 50, m_hwnd, (HMENU)ID_BUTTON_3, hInst, 0);
        hButton4 = CreateWindow(L"Button", L"4", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 140, 140, 50, m_hwnd, (HMENU)ID_BUTTON_4, hInst, 0);
        hButton5 = CreateWindow(L"Button", L"5", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 155, 140, 140, 50, m_hwnd, (HMENU)ID_BUTTON_5, hInst, 0);
        hButton6 = CreateWindow(L"Button", L"6", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 140, 140, 50, m_hwnd, (HMENU)ID_BUTTON_6, hInst, 0);
        hButton7 = CreateWindow(L"Button", L"7", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 85, 140, 50, m_hwnd, (HMENU)ID_BUTTON_7, hInst, 0);
        hButton8 = CreateWindow(L"Button", L"8", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 155, 85, 140, 50, m_hwnd, (HMENU)ID_BUTTON_8, hInst, 0);
        hButton9 = CreateWindow(L"Button", L"9", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 85, 140, 50, m_hwnd, (HMENU)ID_BUTTON_9, hInst, 0);
        hButton10 = CreateWindow(L"Button", L"+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 140, 80, 50, m_hwnd, (HMENU)ID_BUTTON_ADD, hInst, 0);
        hButton11 = CreateWindow(L"Button", L"-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 195, 80, 50, m_hwnd, (HMENU)ID_BUTTON_MIN, hInst, 0);
        hButton12 = CreateWindow(L"Button", L"*", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 250, 80, 50, m_hwnd, (HMENU)ID_BUTTON_MUL, hInst, 0);
        hButton13 = CreateWindow(L"Button", L"/", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 305, 80, 50, m_hwnd, (HMENU)ID_BUTTON_DEL, hInst, 0);
        hButton14 = CreateWindow(L"Button", L"=", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 305, 515, 50, m_hwnd, (HMENU)ID_BUTTON_MAIN, hInst, 0);
        hButton15 = CreateWindow(L"Button", L"C", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 445, 85, 80, 50, m_hwnd, (HMENU)ID_BUTTON_REM, hInst, 0);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton3, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton4, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton5, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton6, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton7, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton8, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton9, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton10, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton11, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton12, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton13, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton14, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SendMessage(hButton15, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

        // Создание и загрузка иконки
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MWND_CALC_ICON));
        if (hIcon != NULL)
        {
            // Установка иконки для большого размера
            SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        }

        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }

        return 0;
    }

    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_SIZE:
        Resize();
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}