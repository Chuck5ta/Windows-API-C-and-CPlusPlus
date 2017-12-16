#include <Windows.h>
#include <ctime> // for the timer (WM_TIMER)
#include <string>

// Decrease ammo code location and opcodes
bool bAmmoStatus = false;
char AmmoOpCode[] = "\xFF\x06"; // \xFF\x06 to INC or use \x90\x90 for NOP
char AmmoDefaultOpCode[] = "\xFF\x0E"; // DEC
DWORD AmmoAddress = 0x4637E9; // address of the decrease ammo code (dec [eax])

							  // handles to the windows/controls
HINSTANCE hInstance; // will be the DLL's handle
HWND hMainhwnd; // handle for the main window
HWND hTitleText; // handle for the title text
HWND hDLLStatuseText; // handle for the injection status text
HWND hFreezeAmmoText; // handle for the Freeze ammo text
HWND hFreezeAmmoStatusText; // handle for the Freeze ammo status text

							// define colours to use for the winmdow's background colour
HBRUSH hBrushLightGrey = CreateSolidBrush(RGB(200, 200, 200));
HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));

// Font definitions
HFONT fontArial = CreateFont(40, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));
HFONT fontArial2 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));

// our timer
UINT TimmerID = 0; // this is used to get the hack to be applied every n seconds

				   // these are used to ID the controls/windows, and are used to execute an event (e.g. click)
#define IDT_TIMER1 (WM_APP + 101)



void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long oldProtection;
	VirtualProtect((LPVOID)addressToWrite, byteNum, PAGE_EXECUTE_READWRITE, &oldProtection);

	memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

	VirtualProtect((LPVOID)addressToWrite, byteNum, oldProtection, NULL);
}


/*
This function deals with all the messages sent to the GUI
*/
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_TIMER: // Process Unlimited Money button event
	{
		if (GetAsyncKeyState(VK_F1))
		{
			bAmmoStatus = !bAmmoStatus;
			if (bAmmoStatus)
			{
				// write to memory
				SendMessage(hFreezeAmmoStatusText, WM_SETTEXT, 0, (LPARAM)"ON");
				WriteToMemory(AmmoAddress, AmmoOpCode, 2); // 2 = number of bytes being written (FF 06)
			}
			else
			{
				SendMessage(hFreezeAmmoStatusText, WM_SETTEXT, 0, (LPARAM)"OFF");
				WriteToMemory(AmmoAddress, AmmoDefaultOpCode, 2); // 2 = number of bytes being written (FF 06)
			}
		}
	}
	break;
	case WM_CREATE: // this is needed for any of the windows to be displayed, even the main window
	{
		// Define a text control (Static control)  			
		// ======================================
		hDLLStatuseText = CreateWindowEx(NULL,
			"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
			"AssaultCube DLL",
			WS_CHILD | WS_VISIBLE,
			100, // x
			20, // y
			290, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hDLLStatuseText == NULL)
			MessageBox(hwnd, "Could not create main title static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hDLLStatuseText, WM_SETFONT, (WPARAM)fontArial, MAKELPARAM(true, 0));

		// DLL Injection status text control  			
		// =================================
		hTitleText = CreateWindowEx(NULL,
			"STATIC", // Static window class 
			"DLL Status: Injected",
			WS_CHILD | WS_VISIBLE,
			90, // x
			80, // y
			290, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hTitleText == NULL)
			MessageBox(hwnd, "Could not create DLL Status static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hTitleText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

		// Freeze ammo text control  			
		// ========================
		hFreezeAmmoText = CreateWindowEx(NULL,
			"STATIC", // Static window class
			"[F1] Freeze ammo:",
			WS_CHILD | WS_VISIBLE,
			30, // x
			140, // y
			290, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hFreezeAmmoText == NULL)
			MessageBox(hwnd, "Could not create Freeze ammo static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hFreezeAmmoText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

		// Freeze ammo text control  			
		// ========================
		hFreezeAmmoText = CreateWindowEx(NULL,
			"STATIC", // Static window class
			"[F1] Freeze ammo:",
			WS_CHILD | WS_VISIBLE,
			30, // x
			140, // y
			290, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hFreezeAmmoText == NULL)
			MessageBox(hwnd, "Could not create Freeze ammo static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hFreezeAmmoText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

		// Freeze ammo status text control  			
		// ===============================
		hFreezeAmmoStatusText = CreateWindowEx(NULL,
			"STATIC", // Static window class 
			"OFF",
			WS_CHILD | WS_VISIBLE,
			250, // x
			140, // y
			290, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hFreezeAmmoStatusText == NULL)
			MessageBox(hwnd, "Could not create Freeze ammo status static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hFreezeAmmoStatusText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));
	}
	break;
	case WM_CTLCOLORSTATIC: // need this to change/set the background colour of the static controls (e.g. title, labels, etc)
	{   // this will change the text and background colour of all Static Control (text/labels)
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255)); // Change the text colour to white
		SetBkColor(hdcStatic, RGB(0, 0, 0));   // change the background colout to black
		return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


// How to create a winapi window in a DLL
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	// WINAPI window

	// Define window
	//---------------
	// Defining the main window class  
	WNDCLASSEX wc = { 0 }; // window class for the main window
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0; // NULL
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(hBrushBlack);
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
	hMainhwnd = CreateWindowEx(0,
		"mainWindowClass",
		"Title",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		50, 50, 500, 400,
		0, 0, hInstance, 0);

	if (hMainhwnd == NULL) // did the creation of the main window work?
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	// display the main window to the screen
	ShowWindow(hMainhwnd, SW_SHOW);
	UpdateWindow(hMainhwnd);

	// set up a timer to fire every n seconds and write to the memory (hack the memory)
	TimmerID = SetTimer(hMainhwnd, IDT_TIMER1, 500, NULL); // timer fires every 1/2 a second

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
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hModule;
		CreateThread(0, NULL, ThreadProc, (LPVOID)"AssaultCube trainer", NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}