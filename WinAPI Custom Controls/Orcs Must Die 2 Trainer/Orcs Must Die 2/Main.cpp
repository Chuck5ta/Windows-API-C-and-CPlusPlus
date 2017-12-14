/*
Trainer for Orcs Must Die 2
Game version: 1.0.0.349 (developed on the steam version)

Author: Chuck E (aka Chucksta, Chuck5ta, ChuckyEgg)
Date: 23rd of January, 2012



Static address         Offsets                Value Written to Memory (Little Endian)   Value Type     Max Value (DEC)
Money		0x00B92450		0xA3, 0x1C, 0x0, 0x0			{0xA3, 0x1C, 0x0, 0x0}					BYTE
Skulls		0x00BA630C		none							{0xF4, 0x01, 0x0, 0x0}					BYTE
Mana		0x00B92458		0x0, 0x310, 0x5C				0x64									FLOAT			100
Health		0x00B92450		0x20, 0x18, 0x54				0xC8									FLOAT			200

This has been built on the trainer from Fleep's tutorials.

Development:
- Windows 7 64bit O/S
- Language: C++ (no OO in this version, pretty much just C++ in name)
- API: Windows
That's it, nothing else, no fancy pants library or IDE or resource :P

What you need in order to be able to run this:
- MS Windows 32bit/64bit O/S
- Orcs Must Die 2, version 1.0.0.349

Dedication:
Fleep, for such an excellent site :-)
Anyone who puts out useful information on the web, that we can actually understand. Thanks to them I could use the Windows API to create
this trainer.
*/

#include <iostream>
#include <Windows.h>
#include <string>
// used as a timer
#include <ctime>

// these are used to ID the controls/windows, and are used to execute an event (e.g. click)
#define IDC_CLOSE_BUTTON 110
#define ID_HEALTH_BUTTON 111
#define ID_MONEY_BUTTON 112
#define ID_MANA_BUTTON 113
#define ID_SKULLS_BUTTON 114
#define ID_GAME_CONNECTION_STATE 115
#define IDT_TIMER1 116

// global variables
HWND Mainhwnd; // handle for the main window
HBITMAP bmpMainBackground = NULL;  // holds the image that will be used as the main window's background
HWND hBtnClose; // exit button's handle
HBITMAP bmpExitBtnBackground = NULL;  // holds the image that will be used as the main window's background
HWND hBtnHealth; // Health button's handle
HWND hBtnMoney; // exit button's handle
HWND hBtnMana; // Health button's handle
HWND hBtnSkulls; // exit button's handle
HWND hBtnGameConnectionState; // handle for the control that shows the game state (connected or not)
HBITMAP bmpOnBtnBackground = NULL;  // holds the image that will be used as the main window's background
HBITMAP bmpOffBtnBackground = NULL;  // holds the image that will be used as the main window's background
HBITMAP bmpGameConnected = NULL;  // holds the image that will be used to show that the trainer is connected to the game
HBITMAP bmpNotGameConnected = NULL;  // holds the image that will be used to show that the trainer is not connected to the game


									 // variables for the memory hack
									 // =============================
std::string GameName = "Orcs Must Die 2";
// NEED to set Character Set to "Use Multi-byte character set"
// - <RIGHT click> project name in Solution Explorer -> Properties
LPCSTR LGameWindow = "Orcs Must Die 2"; //<- MAKE SURE it matches the window name
std::string GameStatus;


bool IsGameAvail = false;
bool exitBTNClicked = false;
bool healthBTnStateIsOn = false;
bool moneyBTnStateIsOn = false;
bool manaBTnStateIsOn = false;
bool skullsBTnStateIsOn = false;
bool gameConnected = false;

// Health related variables
bool HealthStatus;
FLOAT HealthValue = 0xC8; // 100 dec
DWORD HealthBaseAddress = { 0x00B92450 };
DWORD HealthOffsets[] = { 0x20, 0x18, 0x54 }; // 3 level pointer
											  // Money related variables
bool bMoneyButton = false;
bool MoneyStatus = false;
BYTE MoneyValue[] = { 0x80, 0x38, 0x01, 0x0 };
DWORD MoneyBaseAddress = { 0x00B92450 };
DWORD MoneyOffsets[] = { 0x20, 0x18, 0x7C }; // 3 level pointer
											 // Mana related variables
bool ManaStatus;
FLOAT ManaValue = 0x64; // 100 dec
DWORD ManaBaseAddress = { 0x00B92458 };
DWORD ManaOffsets[] = { 0x0, 0x310, 0x5C }; // 3 level pointer
											// Skulls related variables
bool SkullsStatus;
BYTE SkullsValue[] = { 0x20, 0x03, 0x0, 0x0 };
DWORD SkullsBaseAddress = { 0x00BA630C };
DWORD SkullsOffsets[] = { 0x0 }; // 1 level pointer

								 // Process related variables
DWORD dwProcId = NULL; // the process ID 
HANDLE hProcHandle = NULL; // process handle

UINT TimmerID = 0; // used to get the hack to be applied every n seconds

				   //Handles Dynamic memory allocation
				   //Receives how high the pointer level is e.g. 4 levels and from that calculates the initial address
				   //the offset values and the memory addresses for those offsets
DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress)
{
	//DECLARE BASE ADDRESS
	DWORD pointer = BaseAddress;             // Declare a pointer of DWORD
											 //USED TO output the contents in the pointer
	DWORD pTemp;

	DWORD pointerAddr;
	for (int i = 0; i < PointerLevel; i++)
	{
		if (i == 0)
		{
			ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
			//ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, 4, NULL);
		}
		//add first offset to that address
		pointerAddr = pTemp + Offsets[i];   // Set p1 to content of p + offset

											//Read memory one more time and exit the loop
		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, sizeof(pTemp), NULL);
	}
	return pointerAddr;
} // END OF FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress)


  /*
  This method applies the actual hack. It writes the value we want to memory.
  */
void WriteToMemory(HANDLE hProcHandle)
{
	if (healthBTnStateIsOn)
	{
		DWORD HealthAddressToWrite = FindDmaAddy(3, hProcHandle, HealthOffsets, HealthBaseAddress);
		WriteProcessMemory(hProcHandle, (BYTE*)HealthAddressToWrite, &HealthValue, sizeof(FLOAT), NULL);
	}
	if (moneyBTnStateIsOn)
	{
		DWORD MoneyAddressToWrite = FindDmaAddy(3, hProcHandle, MoneyOffsets, MoneyBaseAddress);
		WriteProcessMemory(hProcHandle, (BYTE*)MoneyAddressToWrite, &MoneyValue, sizeof(MoneyValue), NULL);
	}
	if (manaBTnStateIsOn)
	{
		DWORD ManaAddressToWrite = FindDmaAddy(3, hProcHandle, ManaOffsets, ManaBaseAddress);
		WriteProcessMemory(hProcHandle, (BYTE*)ManaAddressToWrite, &ManaValue, sizeof(FLOAT), NULL);
	}
	if (skullsBTnStateIsOn)
	{
		// note, the call to FindDmaAddy() is not required, because there are no offsets to deal with
		WriteProcessMemory(hProcHandle, (BYTE*)SkullsBaseAddress, &SkullsValue, sizeof(SkullsValue), NULL);
	}
} // END OF WriteToMemory(HANDLE hProcHandle)


  /*
  This function checks to see if the game we wish to hack is running
  */
bool checkIfGameIsAvailable(HWND hwnd)
{
	//Declare game unavailable by default
	//if it is available then it will change immediately
	IsGameAvail = false;
	//Check a valid window is available
	// Get Game Window Handle 
	HWND hGameWindow = FindWindow(NULL, LGameWindow);

	//Declare game unavailable by default
	//if it is available then it will change immediately
	IsGameAvail = false;
	//Check a valid window is available
	// Get Window Handle 
	hGameWindow = FindWindow(NULL, LGameWindow);

	if (hGameWindow) // if the window exists
	{
		GetWindowThreadProcessId(hGameWindow, &dwProcId);
		// If it is a valid ID, we continue to try and open the process
		if (dwProcId != 0)
		{
			// Get process Handle
			hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
			if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL)
			{
				// do nothing here, because it has failed
			}
			else
			{
				// trainer is connected to the game, show this in the Trainer window
				gameConnected = true;
				InvalidateRect(hBtnGameConnectionState, 0, 1); // FORCE the redrawing of the Game Connection State control
				return true;
			}
		}
	}
	// we've gotten to this point, which means the game is not available
	gameConnected = false;
	InvalidateRect(hBtnGameConnectionState, 0, 1); // FORCE the redrawing of the Game Connection State control
	return false;
} // END OF checkIfGameIsAvailable(HWND hwnd)


  // this method executes the required hack
void applyHacks(HWND hwnd)
{
	IsGameAvail = checkIfGameIsAvailable(hwnd);

	// This section activates and deactivates the topping up of the money and skulls
	if (IsGameAvail)
	{
		//	if (bMoneyButton || bSkullsButton || bManaButton || HealthStatus)
		if (healthBTnStateIsOn || moneyBTnStateIsOn || manaBTnStateIsOn || skullsBTnStateIsOn)
		{
			// write to memory to apply the hack(s)
			WriteToMemory(hProcHandle);
		}
	}
} // END OF applyHacks(HWND hwnd)


  /*
  This method deals with all the messages sent to the GUI
  */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// TIMER
		// purpose - execute the write to memory (hacks) every n seconds
		case WM_TIMER: // Process Unlimited Money button event
		{
			// apply hacks, if they need to be applied
			applyHacks(hwnd);
		}
		break;
		case WM_CREATE: // this is needed for any of the windows to be displayed, even the main window
		{
			// This loads the image. ready to be displayed (written to a window's/control's surface)
			bmpMainBackground = (HBITMAP)LoadImage(NULL, "MainWindow.bmp", IMAGE_BITMAP, 0, 0,
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (bmpMainBackground == NULL)
				MessageBox(hwnd, "Could not load MainWindow BMP!", "Error", MB_OK | MB_ICONEXCLAMATION);

			// EXIT Button - will be removing the title bar, so will need this
			bmpExitBtnBackground = (HBITMAP)LoadImage(NULL, "ExitButton.bmp", IMAGE_BITMAP, 0, 0,
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (bmpExitBtnBackground == NULL)
				MessageBox(hwnd, "Could not load Exit Button BMP!", "Error", MB_OK | MB_ICONEXCLAMATION);

			// MONEY/ Button -On state
			bmpOnBtnBackground = (HBITMAP)LoadImage(NULL, "OnBtn.bmp", IMAGE_BITMAP, 0, 0,
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (bmpOnBtnBackground == NULL)
				MessageBox(hwnd, "Could not load On Button BMP!", "Error", MB_OK | MB_ICONEXCLAMATION);

			// MONEY Button -On state
			bmpOffBtnBackground = (HBITMAP)LoadImage(NULL, "OffBtn.bmp", IMAGE_BITMAP, 0, 0,
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (bmpOffBtnBackground == NULL)
				MessageBox(hwnd, "Could not load Off Button BMP!", "Error", MB_OK | MB_ICONEXCLAMATION);

			// Game Connected
			bmpGameConnected = (HBITMAP)LoadImage(NULL, "GameConnected.bmp", IMAGE_BITMAP, 0, 0,
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (bmpGameConnected == NULL)
				MessageBox(hwnd, "Could not load Game Connected BMP!", "Error", MB_OK | MB_ICONEXCLAMATION);

			// Game NOT Connected
			bmpNotGameConnected = (HBITMAP)LoadImage(NULL, "GameNotConnected.bmp", IMAGE_BITMAP, 0, 0,
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (bmpNotGameConnected == NULL)
				MessageBox(hwnd, "Could not load Game NOT Connected BMP!", "Error", MB_OK | MB_ICONEXCLAMATION);

			// DEFINE A BUTTON - EXIT program 
			// ==============================
			hBtnClose = CreateWindowEx(NULL,
				"BUTTON",
				"",
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				590,
				12,
				70,
				30,
				hwnd,
				(HMENU)IDC_CLOSE_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnClose == NULL)
				MessageBox(hwnd, "Could not create button.", "Error", MB_OK | MB_ICONERROR);
			/* Set Exit button's image */
			//		SendMessage(hBtnClose, BM_SETIMAGE, IMAGE_BITMAP,
			//			(LPARAM)LoadImage( NULL, "ExitButton.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE ));
			// END OF Exit button's definition

			// DEFINE A HEALTH BUTTON  
			// ======================
			hBtnHealth = CreateWindowEx(NULL,
				"BUTTON",
				"",
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				545,
				180,
				123,
				42,
				hwnd,
				(HMENU)ID_HEALTH_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnHealth == NULL)
				MessageBox(hwnd, "Could not create health button.", "Error", MB_OK | MB_ICONERROR);

			// DEFINE A MONEY BUTTON  
			// =====================
			hBtnMoney = CreateWindowEx(NULL,
				"BUTTON",
				"",
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				545,
				239,
				123,
				42,
				hwnd,
				(HMENU)ID_MONEY_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnMoney == NULL)
				MessageBox(hwnd, "Could not create money button.", "Error", MB_OK | MB_ICONERROR);

			// DEFINE A MANA BUTTON  
			// ====================
			hBtnMana = CreateWindowEx(NULL,
				"BUTTON",
				"",
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				545,
				295,
				123,
				42,
				hwnd,
				(HMENU)ID_MANA_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnMana == NULL)
				MessageBox(hwnd, "Could not create mana button.", "Error", MB_OK | MB_ICONERROR);

			// DEFINE A SKULLS BUTTON  
			// ======================
			hBtnSkulls = CreateWindowEx(NULL,
				"BUTTON",
				"",
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				545,
				352,
				123,
				42,
				hwnd,
				(HMENU)ID_SKULLS_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnSkulls == NULL)
				MessageBox(hwnd, "Could not create skulls button.", "Error", MB_OK | MB_ICONERROR);

			// DEFINE THE GAME CONNECTION STATE CONTROL  
			// ========================================
			hBtnGameConnectionState = CreateWindowEx(NULL,
				"BUTTON",
				"",
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				0,
				169,
				203,
				145,
				hwnd,
				(HMENU)ID_GAME_CONNECTION_STATE,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnGameConnectionState == NULL)
				MessageBox(hwnd, "Could not create Game Connection State control.", "Error!", MB_OK | MB_ICONERROR);

			// Create the icon that will appear in the taskbar
			// ===============================================
			HICON hIcon, hIconSm;
			hIcon = (HICON)LoadImage(NULL, "OMD2Trainer.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
			if (hIcon)
			{
				SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			}
			else
			{
				MessageBox(hwnd, "Could not load large icon! Is it in the current working directory?", "Error", MB_OK | MB_ICONERROR);
			}
			hIconSm = (HICON)LoadImage(NULL, "OMD2Trainer.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
			if (hIconSm)
			{
				SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
			}
			else
			{
				MessageBox(hwnd, "Could not load small icon! Is it in the current working directory?", "Error", MB_OK | MB_ICONERROR);
			}

		}
		break;

		case WM_DRAWITEM: // change the image of a button when the image is supposed to change
		{
			// access the structure that holds the GUI components
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
			HDC hdcMem = CreateCompatibleDC(lpdis->hDC);

			//     HEALTH BUTTON
			// changing the Money button's text (On/Off)
			if (lpdis->hwndItem == hBtnHealth && healthBTnStateIsOn) // checks to see if the current control we are handling an evet for is the Health button
			{
				SelectObject(hdcMem, bmpOnBtnBackground);  // On
			}
			if (lpdis->hwndItem == hBtnHealth && !healthBTnStateIsOn)
			{
				SelectObject(hdcMem, bmpOffBtnBackground); // Off
			}

			//     MONEY BUTTON
			// changing the Money button's text (On/Off)
			if (lpdis->hwndItem == hBtnMoney && moneyBTnStateIsOn) // checks to see if the current control we are handling an evet for is the Money button
			{
				SelectObject(hdcMem, bmpOnBtnBackground);  // On
			}
			if (lpdis->hwndItem == hBtnMoney && !moneyBTnStateIsOn)
			{
				SelectObject(hdcMem, bmpOffBtnBackground); // Off
			}

			//     MANA BUTTON
			// changing the Mana button's text (On/Off)
			if (lpdis->hwndItem == hBtnMana && manaBTnStateIsOn) // checks to see if the current control we are handling an event for is the Mana button
			{
				SelectObject(hdcMem, bmpOnBtnBackground);  // On
			}
			if (lpdis->hwndItem == hBtnMana && !manaBTnStateIsOn)
			{
				SelectObject(hdcMem, bmpOffBtnBackground); // Off
			}

			//     SKULLS BUTTON
			// changing the Skulls button's text (On/Off)
			if (lpdis->hwndItem == hBtnSkulls && skullsBTnStateIsOn) // checks to see if the current control we are handling an event for is the Skulls button
			{
				SelectObject(hdcMem, bmpOnBtnBackground);  // On
			}
			if (lpdis->hwndItem == hBtnSkulls && !skullsBTnStateIsOn)
			{
				SelectObject(hdcMem, bmpOffBtnBackground); // Off
			}

			//     GAME CONNECTION STATE
			// changing the game connection state image to show as being connected
			if (lpdis->hwndItem == hBtnGameConnectionState && gameConnected) // checks to see if the current control we are handling an event for is the Game Connection State control
			{
				SelectObject(hdcMem, bmpGameConnected);  // On
			}
			// changing the game connection state image to show as being NOT connected
			if (lpdis->hwndItem == hBtnGameConnectionState && !gameConnected)
			{
				SelectObject(hdcMem, bmpNotGameConnected); // Off
			}

			// Draw the image of the control (whichever we are dealing with - Health, Money, Mana, or Skulls)
			BitBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, lpdis->rcItem.bottom - lpdis->rcItem.top, hdcMem, 0, 0, SRCCOPY);
			//	StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right - lpdis->rcItem.left, lpdis->rcItem.bottom - lpdis->rcItem.top, hdcMem, 0, 0, 0, 0, SRCCOPY);
			DeleteDC(hdcMem);
			return TRUE;
		}
		break;
		case WM_PAINT:
		{
			// THIS PAINTS THE IMAGE MAIN WINDOW 
			// =================================
			BITMAP bm;
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(Mainhwnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, bmpMainBackground);

			GetObject(bmpMainBackground, sizeof(bm), &bm);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			EndPaint(Mainhwnd, &ps);
			// END OF PAINT IMAGE TO THE MAIN WINDOW ???ND OF PAINT IMAGE TO THE Exit buton

			// PAINT THE EXIT BUTTON'S IMAGE
			// =============================
			BITMAP bmExitBtn;
			PAINTSTRUCT psExitBtn;

			HDC hdcExitBtn = BeginPaint(hBtnClose, &psExitBtn);

			HDC hdcMemExitBtn = CreateCompatibleDC(hdcExitBtn);
			HBITMAP hbmOldExitBtn = (HBITMAP)SelectObject(hdcMemExitBtn, bmpExitBtnBackground);

			GetObject(bmpExitBtnBackground, sizeof(bmExitBtn), &bmExitBtn);

			BitBlt(hdcExitBtn, 0, 0, bmExitBtn.bmWidth, bmExitBtn.bmHeight, hdcMemExitBtn, 0, 0, SRCCOPY);

			SelectObject(hdcMemExitBtn, hbmOldExitBtn);
			DeleteDC(hdcMemExitBtn);

			EndPaint(hBtnClose, &psExitBtn);
			// END OF PAINT THE EXIT BUTTON'S IMAGE

			// BUTTON IMAGE PAINTING IS HANDLED BY WM_DRAWITEM
		}
		break;
		case WM_COMMAND:   // processes Menu commands and window events
			switch (LOWORD(wParam))
			{
			case IDC_CLOSE_BUTTON: // Exit the application
			{
				// exit the application
				DestroyWindow(hwnd);
			}
			break;
			case ID_HEALTH_BUTTON: // switch the Money button between On and Off
			{
				// set the state of the button
				healthBTnStateIsOn = !healthBTnStateIsOn;
				InvalidateRect(hBtnHealth, 0, 1); // FORCE the redrawing of the Money button
			}
			break;
			case ID_MONEY_BUTTON: // switch the Money button between On and Off
			{
				// set the state of the button
				moneyBTnStateIsOn = !moneyBTnStateIsOn;
				InvalidateRect(hBtnMoney, 0, 1); // FORCE the redrawing of the Money button
			}
			break;
			case ID_MANA_BUTTON: // switch the Mana button between On and Off
			{
				// set the state of the button
				manaBTnStateIsOn = !manaBTnStateIsOn;
				InvalidateRect(hBtnMana, 0, 1); // FORCE the redrawing of the Mana button
			}
			break;
			case ID_SKULLS_BUTTON: // switch the Skulls button between On and Off
			{
				// set the state of the button
				skullsBTnStateIsOn = !skullsBTnStateIsOn;
				InvalidateRect(hBtnSkulls, 0, 1); // FORCE the redrawing of the Skulls button
			}
			break;
			}
			break;
		case WM_DESTROY: // REQUIRED for killing the program's process
			PostQuitMessage(0);
			break;
		case WM_NCHITTEST: // Make the client area of the main window draggable, as we will remove the borders and title bar
		{
			LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
			if (hit == HTCLIENT) hit = HTCAPTION;
			return hit;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
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
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
		"Orcs Must Die 2 trainer",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		50, 50, 678, 425,
		0, 0, hInstance, 0);

	if (Mainhwnd == NULL) // did the creation of the main window work?
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}


	// remove the borders and title bar
	// ================================
	LONG lStyle = GetWindowLong(Mainhwnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLong(Mainhwnd, GWL_STYLE, lStyle);
	// remove the extended borders
	LONG lExStyle = GetWindowLong(Mainhwnd, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	SetWindowLongPtr(Mainhwnd, GWL_EXSTYLE, lExStyle);
	// redisplay window after removing borders and title bar
	SetWindowPos(Mainhwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	// display the main window to the screen
	ShowWindow(Mainhwnd, nCmdShow);
	UpdateWindow(Mainhwnd);

	// set up a timer to fire every n seconds and write to the memory (hack the memory)
	TimmerID = SetTimer(Mainhwnd, IDT_TIMER1, 500, NULL); // timer fires every 1/2 a second

	// Step 3: The Message Loop
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