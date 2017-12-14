/*
This is an example of the creation of the main window for a Win32 application
*/
#include <Windows.h>

// global variables
HWND Mainhwnd; // handle for the main window
HWND hTitleText; // handle for the label

				 // Colour definitions
				 // for RGB values: https://www.rapidtables.com/web/color/RGB_Color.html
HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hBrushLightGrey = CreateSolidBrush(RGB(200, 200, 200));

// Font definitions
// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd183499(v=vs.85).aspx
HFONT fontCourierNew = CreateFont(-17, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, "Courier New");
HFONT fontTimesNewRoman = CreateFont(36, 20, -300, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
HFONT fontImpact = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
HFONT fontArial = CreateFont(40, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));
HFONT fontArial2 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));

/*
This function deals with all the messages sent to the GUI
*/
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE: // this is needed for any of the windows to be displayed, even the main window
	{
		// Define a text control (Static control)  			
		// ======================================
		hTitleText = CreateWindowEx(NULL,
			"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
			"This is a label",
			WS_CHILD | WS_VISIBLE,
			120, // x
			30, // y
			370, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hTitleText == NULL)
			MessageBox(hwnd, "Could not create static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hTitleText, WM_SETFONT, (WPARAM)fontArial, MAKELPARAM(true, 0));
	}
	break;
	case WM_CTLCOLORSTATIC: // need this to change/set the background colour of the static controls (e.g. title, labels, etc)
	{   // this will change the text and background colour of all Static Control (text/labels)
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255)); // Change the text colour to white
		SetBkColor(hdcStatic, RGB(200, 200, 200));   // change the background colout to light grey
		return (INT_PTR)CreateSolidBrush(RGB(200, 200, 200));
	}
	break;
	case WM_DESTROY: // REQUIRED in order to close the program completely
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0; // gets to here means something went wrong
} // END OF WindowProcedure(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR args, int nCmdShow)
{
	WNDCLASSEX wc = { 0 }; // window class for the main window
	MSG Msg;

	// Defining the main window class   

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0; // NULL
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(hBrushLightGrey);
	wc.lpszMenuName = 0; // NULL
	wc.lpszClassName = "mainWindowClass";
	wc.hIconSm = 0; // NULL

					// registering the main window class
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Creating the main window
	Mainhwnd = CreateWindowEx(0,
		"mainWindowClass",
		"Title",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		50, 50, 678, 425,
		0, 0, hInstance, 0);

	if (Mainhwnd == NULL) // did the creation of the main window work?
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// display the main window to the screen
	ShowWindow(Mainhwnd, nCmdShow);
	UpdateWindow(Mainhwnd);

	// The Message Loop
	// this processes all messages (mouse, keyboard) going to the window
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		// process certain keyboard events (e.g. WM_CHAR, WM_KEYDOWN)
		TranslateMessage(&Msg);
		// send the processed message out to the window
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
} // END OF WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR args,int nCmdShow)
