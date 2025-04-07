#include <windows.h>

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

// WindowSpecification struct to hold window properties
struct WindowSpecification
{
	const char* title;
	int width;
	int height;
};

int main()
{
	// Step 1: Define window specifications using the struct
	const WindowSpecification windowSpec = { "PortLink", 800, 600 };

	// Step 2: Define the window class
	const char WindowClassName[] = "MainWindowClass";

	WNDCLASS windowClass = {};
	windowClass.lpfnWndProc = WindowProc;          // Set the window procedure function
	windowClass.hInstance = GetModuleHandle(nullptr);  // Application instance handle
	windowClass.lpszClassName = WindowClassName;   // Window class name
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW); // Default cursor

	// Register the window class
	if (!RegisterClass(&windowClass))
		return 0;

	// Step 3: Create the window using values from the struct
	HWND hwnd = CreateWindowEx(
		0,                                    // Optional window styles
		WindowClassName,                      // Window class name
		windowSpec.title,                     // Window title from struct ("PortLink")
		WS_OVERLAPPEDWINDOW,                  // Window style
		CW_USEDEFAULT, CW_USEDEFAULT,         // Position (default)
		windowSpec.width, windowSpec.height,  // Size (width, height)
		nullptr,                              // Parent window
		nullptr,                              // Menu
		windowClass.hInstance,                // Instance handle
		nullptr                               // Additional application data
	);

	if (hwnd == nullptr)
		return 0;

	// Step 4: Show and update the window
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	// Step 5: Main message loop
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
