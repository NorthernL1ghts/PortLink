#define UNICODE
#define _UNICODE

#include <windows.h>
#include <dshow.h>
#include <vector>
#include <string>

#pragma comment(lib, "strmiids.lib")

// Global variables
std::vector<std::wstring> g_WebcamNames;
IGraphBuilder* g_GraphBuilder = nullptr;
IMediaControl* g_MediaControl = nullptr;
IBaseFilter* g_CaptureFilter = nullptr;
bool g_IsRunning = true;

// Enumerate connected webcam devices
void EnumerateWebcamDevices() {
    g_WebcamNames.clear();

    ICreateDevEnum* p_DeviceEnumerator = nullptr;
    IEnumMoniker* p_EnumMoniker = nullptr;

    if (SUCCEEDED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&p_DeviceEnumerator))) {
        if (SUCCEEDED(p_DeviceEnumerator->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &p_EnumMoniker, 0)) && p_EnumMoniker) {
            IMoniker* p_Moniker = nullptr;
            while (p_EnumMoniker->Next(1, &p_Moniker, nullptr) == S_OK) {
                IPropertyBag* p_PropertyBag;
                if (SUCCEEDED(p_Moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void**)&p_PropertyBag))) {
                    VARIANT variantName;
                    VariantInit(&variantName);
                    if (SUCCEEDED(p_PropertyBag->Read(L"FriendlyName", &variantName, nullptr)))
                        g_WebcamNames.push_back(variantName.bstrVal);
                    VariantClear(&variantName);
                    p_PropertyBag->Release();
                }
                p_Moniker->Release();
            }
            p_EnumMoniker->Release();
        }
        p_DeviceEnumerator->Release();
    }
}

// Initialize webcam capture and rendering
bool StartWebcamPreview(HWND h_ParentWindow) {
    HRESULT handleResult = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&g_GraphBuilder);
    if (FAILED(handleResult)) return false;

    ICreateDevEnum* p_DeviceEnumerator = nullptr;
    handleResult = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&p_DeviceEnumerator);
    if (FAILED(handleResult)) return false;

    IEnumMoniker* p_EnumMoniker = nullptr;
    handleResult = p_DeviceEnumerator->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &p_EnumMoniker, 0);
    if (FAILED(handleResult) || !p_EnumMoniker) return false;

    IMoniker* p_Moniker = nullptr;
    if (p_EnumMoniker->Next(1, &p_Moniker, nullptr) == S_OK) {
        handleResult = p_Moniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&g_CaptureFilter);
        if (SUCCEEDED(handleResult))
            g_GraphBuilder->AddFilter(g_CaptureFilter, L"Webcam Video Capture");
        p_Moniker->Release();
    }

    p_EnumMoniker->Release();
    p_DeviceEnumerator->Release();

    // Create and add video renderer
    IBaseFilter* p_VideoRenderer = nullptr;
    handleResult = CoCreateInstance(CLSID_VideoRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&p_VideoRenderer);
    if (FAILED(handleResult)) return false;

    g_GraphBuilder->AddFilter(p_VideoRenderer, L"Video Renderer");

    // Setup capture graph
    ICaptureGraphBuilder2* p_CaptureGraphBuilder = nullptr;
    handleResult = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&p_CaptureGraphBuilder);
    if (FAILED(handleResult)) return false;

    p_CaptureGraphBuilder->SetFiltergraph(g_GraphBuilder);
    handleResult = p_CaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, g_CaptureFilter, nullptr, p_VideoRenderer);
    if (FAILED(handleResult)) return false;

    // Setup video window
    IVideoWindow* p_VideoWindow = nullptr;
    g_GraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&p_VideoWindow);
    p_VideoWindow->put_Owner((OAHWND)h_ParentWindow);
    p_VideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    p_VideoWindow->SetWindowPosition(0, 0, 800, 600);

    // Start streaming
    g_GraphBuilder->QueryInterface(IID_IMediaControl, (void**)&g_MediaControl);
    g_MediaControl->Run();

    return true;
}

// Monitor ESC key in background to close app
DWORD WINAPI MonitorHotkeyExit(LPVOID p_Param) {
    HWND h_ParentWindow = (HWND)p_Param;

    while (g_IsRunning) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            g_IsRunning = false;
            PostMessage(h_ParentWindow, WM_CLOSE, 0, 0);
            ExitProcess(0);
        }
        Sleep(100);
    }
    return 0;
}

// Main window procedure
LRESULT CALLBACK WindowProc(HWND h_Window, UINT u_Msg, WPARAM wParam, LPARAM lParam) {
    switch (u_Msg) {
    case WM_DESTROY:
        g_IsRunning = false;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(h_Window, u_Msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE h_ApplicationInstance, HINSTANCE h_PreviousInstance, PWSTR p_CommandLineArgs, int n_ShowCommand) {
    CoInitialize(nullptr);

    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = h_ApplicationInstance;
    windowClass.lpszClassName = L"PortLinkWindowClass";

    RegisterClass(&windowClass);

    HWND h_MainWindow = CreateWindowExW(
        0,
        L"PortLinkWindowClass",
        L"PortLink - Webcam Viewer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr, nullptr, h_ApplicationInstance, nullptr
    );

    ShowWindow(h_MainWindow, n_ShowCommand);

    // Start webcam preview automatically
    StartWebcamPreview(h_MainWindow);

    // Launch the hotkey ESC-monitoring thread
    CreateThread(nullptr, 0, MonitorHotkeyExit, h_MainWindow, 0, nullptr);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    CoUninitialize();
    return 0;
}
