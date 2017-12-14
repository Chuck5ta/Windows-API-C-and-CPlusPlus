/*
This is an example of the creation of the winapi button control
2 buttons are implemented.
The first is an Exit button, that closes the program
The second demonstates the changing of text on the button
*/
#include <Windows.h>

// these are used to ID the controls/windows, and are used to execute an event (e.g. click)
#define IDC_CLOSE_BUTTON 110
#define ID_HEALTH_BUTTON 111

// global variables
HWND Mainhwnd; // handle for the main window
HWND hBtnExit; // Exit button's handle
HWND hBtnHackHealth; // Health button's handle

// Colour definitions
// for RGB values: https://www.rapidtables.com/web/color/RGB_Color.html
HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hBrushLightGrey = CreateSolidBrush(RGB(200, 200, 200));

// Flags
bool healthBTnStateIsOn = false;

/*
This function deals with all the messages sent to the GUI
*/
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE: // this is needed for any of the windows to be displayed, even the main window
	{
		// DEFINE A HEALTH BUTTON  
		// - this button demonstrates the changing of text on the button
		hBtnExit = CreateWindowEx(NULL,
			"BUTTON",
			"EXIT",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
			290, // x
			140, // y
			60, // Width
			42,  // Height
			hwnd,
			(HMENU)IDC_CLOSE_BUTTON,
			GetModuleHandle(NULL),
			NULL);
		if (hBtnExit == NULL)
			MessageBox(hwnd, "Could not create Exit button.", "Error", MB_OK | MB_ICONERROR);

		// DEFINE A HEALTH BUTTON  
		// - this button demonstrates the changing of text on the button
		hBtnHackHealth = CreateWindowEx(NULL,
			"BUTTON",
			"Hack Health OFF",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
			260, // x
			200, // y
			123, // Width
			42,  // Height
			hwnd,
			(HMENU)ID_HEALTH_BUTTON,
			GetModuleHandle(NULL),
			NULL);
		if (hBtnHackHealth == NULL)
			MessageBox(hwnd, "Could not create Hack Health button.", "Error", MB_OK | MB_ICONERROR);
	}
	case WM_COMMAND:
		switch (wParam)
		{
			case IDC_CLOSE_BUTTON: // Exit the application
			{
				// exit the application
				DestroyWindow(hwnd);
			}
			break;
			case ID_HEALTH_BUTTON: // switch the Health button between On and Off
			{
				// set the state of the button
				healthBTnStateIsOn = !healthBTnStateIsOn;

				if (healthBTnStateIsOn)
				{
					SendMessage(hBtnHackHealth, WM_SETTEXT, 0, (LPARAM)"Hack Health ON"); // change the text to ON
				}
				else
					SendMessage(hBtnHackHealth, WM_SETTEXT, 0, (LPARAM)"Hack Health OFF"); // change the text to OFF

				InvalidateRect(hBtnHackHealth, 0, 1); // FORCE the redrawing of the Money button
			}
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
