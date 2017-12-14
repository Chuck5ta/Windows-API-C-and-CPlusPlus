/*
This is an example of the creation of an edit control
The Undo (IDM_EDUNDO), Cut (IDM_EDCUT), Copy, Paste, and Del (under WM_COMMAND) are not needed unless you implement a menu with these options
*/
#include <Windows.h>

// these are used to ID the controls/windows, and are used to execute an event (e.g. click)
#define ID_EDITCHILD 110 // The edit control (need one of these for each edit control)
#define IDM_EDUNDO 111
#define IDM_EDCUT 112
#define IDM_EDCOPY 113
#define IDM_EDPASTE 114
#define IDM_EDDEL 115

#define GWL_HINSTANCE 6 // required in the definition of the edit control

// global variables
HWND Mainhwnd; // handle for the main window
HWND hEdit; // handle for the Edit control

// Colour definitions (used to set/change the main window's colour)
// for RGB values: https://www.rapidtables.com/web/color/RGB_Color.html
HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hBrushLightGrey = CreateSolidBrush(RGB(200, 200, 200));

/*
This function deals with all the messages sent to the GUI
*/
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE: // this is needed for any of the windows to be displayed, even the main window
	{
		// Define an Edit control 			
		// ======================
		hEdit = CreateWindowEx(
			NULL, "EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			87,  // x
			81,  // y
			150, // Width
			120, // Height
			hwnd,         // parent window 
			(HMENU)ID_EDITCHILD,   // edit control ID
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);        // pointer not needed
						  // Add text to the window. 
		SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"This is the edit control");
	}
	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_EDUNDO: // Edit control (to be used by menu options)
						 // Send WM_UNDO only if there is something to be undone. 
			if (SendMessage(hEdit, EM_CANUNDO, 0, 0))
				SendMessage(hEdit, WM_UNDO, 0, 0);
			else
			{
				MessageBox(hEdit,
					"Nothing to undo.",
					"Undo notification",
					MB_OK);
			}
			break;
		case IDM_EDCUT: // Edit control (to be used by menu options)
			SendMessage(hEdit, WM_CUT, 0, 0);
			break;

		case IDM_EDCOPY: // Edit control (to be used by menu options)
			SendMessage(hEdit, WM_COPY, 0, 0);
			break;

		case IDM_EDPASTE: // Edit control (to be used by menu options)
			SendMessage(hEdit, WM_PASTE, 0, 0);
			break;

		case IDM_EDDEL: // Edit control (to be used by menu options)
			SendMessage(hEdit, WM_CLEAR, 0, 0);
			break;
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
