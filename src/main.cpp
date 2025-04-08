#define UNICODE
#define _UNICODE

#include <windows.h>
#include <dshow.h>
#include <vector>
#include <string>

#pragma comment(lib, "strmiids.lib")

// Global variables
HWND hWndButton;
std::vector<std::wstring> webcamNames;
IGraphBuilder* pGraph = nullptr;
IMediaControl* pMediaControl = nullptr;
IBaseFilter* pCaptureFilter = nullptr;

// Function to enumerate available webcams
void EnumerateAvailableWebcams() {
    webcamNames.clear();
    ICreateDevEnum* pDevEnum = nullptr;
    IEnumMoniker* pEnum = nullptr;

    if (SUCCEEDED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum))) {
        if (SUCCEEDED(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0)) && pEnum) {
            IMoniker* pMoniker = nullptr;
            while (pEnum->Next(1, &pMoniker, nullptr) == S_OK) {
                IPropertyBag* pBag;
                if (SUCCEEDED(pMoniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void**)&pBag))) {
                    VARIANT varName;
                    VariantInit(&varName);
                    if (SUCCEEDED(pBag->Read(L"FriendlyName", &varName, nullptr))) {
                        webcamNames.push_back(varName.bstrVal);
                    }
                    VariantClear(&varName);
                    pBag->Release();
                }
                pMoniker->Release();
            }
            pEnum->Release();
        }
        pDevEnum->Release();
    }
}

// Function to initialize the webcam and display the video feed
bool InitializeWebcamRendering(HWND hWnd) {
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
    if (FAILED(hr)) return false;

    ICreateDevEnum* pDevEnum = nullptr;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum);
    if (FAILED(hr)) return false;

    IEnumMoniker* pEnum = nullptr;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (FAILED(hr) || !pEnum) return false;

    IMoniker* pMoniker = nullptr;
    if (pEnum->Next(1, &pMoniker, nullptr) == S_OK) {
        hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&pCaptureFilter);
        if (SUCCEEDED(hr)) {
            pGraph->AddFilter(pCaptureFilter, L"Webcam Video Capture");
        }
    }

    pMoniker->Release();
    pEnum->Release();
    pDevEnum->Release();

    // Create video renderer
    IBaseFilter* pVideoRenderer = nullptr;
    hr = CoCreateInstance(CLSID_VideoRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pVideoRenderer);
    if (FAILED(hr)) return false;

    pGraph->AddFilter(pVideoRenderer, L"Video Renderer");

    // Connect the webcam feed to the renderer
    ICaptureGraphBuilder2* pCaptureGraph = nullptr;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pCaptureGraph);
    if (FAILED(hr)) return false;

    pCaptureGraph->SetFiltergraph(pGraph);
    hr = pCaptureGraph->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCaptureFilter, nullptr, pVideoRenderer);
    if (FAILED(hr)) return false;

    // Configure the rendering window with a larger size
    IVideoWindow* pVideoWindow = nullptr;
    pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
    pVideoWindow->put_Owner((OAHWND)hWnd);
    pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

    // Set the window size to be larger for better visibility
    pVideoWindow->SetWindowPosition(0, 0, 800, 600);

    // Start video streaming
    pGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);
    pMediaControl->Run();

    return true;
}

// Handle button clicks
void HandleButtonClick(HWND hWnd) {
    if (InitializeWebcamRendering(hWnd)) {
        MessageBoxW(hWnd, L"Webcam Opened Successfully!", L"PortLink", MB_OK);
    }
    else {
        MessageBoxW(hWnd, L"Failed to Open Webcam", L"PortLink", MB_OK);
    }
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if ((HWND)lParam == hWndButton) HandleButtonClick(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    CoInitialize(nullptr);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"PortLinkWindowClass";
    RegisterClass(&wc);

    HWND hWnd = CreateWindowExW(0, L"PortLinkWindowClass", L"PortLink - Webcam Viewer", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

    hWndButton = CreateWindowW(L"BUTTON", L"Start Webcam", WS_VISIBLE | WS_CHILD, 330, 250, 140, 30, hWnd, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}
