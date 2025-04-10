#define UNICODE
#define _UNICODE

#include <windows.h>
#include <dshow.h>
#include <string>
#include <vector>
#include <ctime>

#pragma comment(lib, "strmiids.lib")

// Global variables
std::vector<std::wstring> g_WebcamNames;
IGraphBuilder* g_GraphBuilder = nullptr;
IMediaControl* g_MediaControl = nullptr;
IBaseFilter* g_CaptureFilter = nullptr;
bool g_IsRunning = true;
HWND g_hClockLabel = nullptr;
#define IDT_CLOCK_TIMER 1

// Webcam enumeration
void EnumerateWebcamDevices() {
    g_WebcamNames.clear();

    ICreateDevEnum* pEnum = nullptr;
    IEnumMoniker* pMonikerEnum = nullptr;

    if (SUCCEEDED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pEnum))) {
        if (SUCCEEDED(pEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pMonikerEnum, 0)) && pMonikerEnum) {
            IMoniker* pMoniker = nullptr;
            while (pMonikerEnum->Next(1, &pMoniker, nullptr) == S_OK) {
                IPropertyBag* pBag;
                if (SUCCEEDED(pMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void**)&pBag))) {
                    VARIANT var;
                    VariantInit(&var);
                    if (SUCCEEDED(pBag->Read(L"FriendlyName", &var, nullptr)))
                        g_WebcamNames.push_back(var.bstrVal);
                    VariantClear(&var);
                    pBag->Release();
                }
                pMoniker->Release();
            }
            pMonikerEnum->Release();
        }
        pEnum->Release();
    }
}

// Start webcam
bool StartWebcamPreview(HWND hParent) {
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&g_GraphBuilder);
    if (FAILED(hr)) return false;

    ICreateDevEnum* pEnum = nullptr;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pEnum);
    if (FAILED(hr)) return false;

    IEnumMoniker* pMonikerEnum = nullptr;
    hr = pEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pMonikerEnum, 0);
    if (FAILED(hr) || !pMonikerEnum) return false;

    IMoniker* pMoniker = nullptr;
    if (pMonikerEnum->Next(1, &pMoniker, nullptr) == S_OK) {
        hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&g_CaptureFilter);
        if (SUCCEEDED(hr)) {
            g_GraphBuilder->AddFilter(g_CaptureFilter, L"Webcam Capture");
        }
        pMoniker->Release();
    }

    pMonikerEnum->Release();
    pEnum->Release();

    IBaseFilter* pVideoRenderer = nullptr;
    hr = CoCreateInstance(CLSID_VideoRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pVideoRenderer);
    if (FAILED(hr)) return false;
    g_GraphBuilder->AddFilter(pVideoRenderer, L"Video Renderer");

    ICaptureGraphBuilder2* pCaptureBuilder = nullptr;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pCaptureBuilder);
    if (FAILED(hr)) return false;

    pCaptureBuilder->SetFiltergraph(g_GraphBuilder);
    hr = pCaptureBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, g_CaptureFilter, nullptr, pVideoRenderer);
    if (FAILED(hr)) return false;

    IVideoWindow* pVideoWindow = nullptr;
    g_GraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
    pVideoWindow->put_Owner((OAHWND)hParent);
    pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    pVideoWindow->SetWindowPosition(0, 0, 800, 600);

    g_GraphBuilder->QueryInterface(IID_IMediaControl, (void**)&g_MediaControl);
    g_MediaControl->Run();

    if (pCaptureBuilder) pCaptureBuilder->Release();
    if (pVideoRenderer) pVideoRenderer->Release();
    if (pVideoWindow) pVideoWindow->Release();

    return true;
}

// Monitor ESC key
DWORD WINAPI MonitorHotkeyExit(LPVOID param) {
    HWND hWnd = (HWND)param;
    while (g_IsRunning) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            g_IsRunning = false;
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            ExitProcess(0);
        }
        Sleep(100);
    }
    return 0;
}

// Update system time in label
void UpdateClockLabel(HWND hwndLabel) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t timeStr[100];
    swprintf_s(timeStr, 100, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    SetWindowText(hwndLabel, timeStr);
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == IDT_CLOCK_TIMER) {
            UpdateClockLabel(g_hClockLabel);
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, IDT_CLOCK_TIMER);
        if (g_MediaControl) g_MediaControl->Release();
        if (g_CaptureFilter) g_CaptureFilter->Release();
        if (g_GraphBuilder) g_GraphBuilder->Release();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// Entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    CoInitialize(nullptr);

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WebcamTimeWindow";
    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0, L"WebcamTimeWindow", L"Webcam Viewer with Time",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);

    // Create clock label
    g_hClockLabel = CreateWindowEx(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        620, 10, 160, 30,
        hWnd, nullptr, hInstance, nullptr);

    HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    SendMessage(g_hClockLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    SetTimer(hWnd, IDT_CLOCK_TIMER, 1000, nullptr);
    UpdateClockLabel(g_hClockLabel);

    StartWebcamPreview(hWnd);
    CreateThread(nullptr, 0, MonitorHotkeyExit, hWnd, 0, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hFont);
    CoUninitialize();
    return 0;
}
