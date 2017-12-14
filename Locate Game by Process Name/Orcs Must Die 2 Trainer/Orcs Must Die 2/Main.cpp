/*
Trainer for Orcs Must Die 2
Game version: 1.0.0.349 (developed on the steam version)

Author: Chuck E (aka Chucksta, Chuck5ta, ChuckyEgg)
Date: 23rd of January, 2012



Static address         Offsets                Value Written to Memory (Little Endian)   Value Type     Max Value (DEC)
Money		0x00B92450		0x20, 0x18, 0x7C                             					 
Skulls		0x00BA630C		none							            					 
Mana		0x00B92458		0x0, 0x310, 0x5C				  								 			 
Health		0x00B92450		0x20, 0x18, 0x54				 									 			 

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
#include <TlHelp32.h> // for searching for processes (PROCESSENTRY32)

// these are used to ID the controls/windows, and are used to execute an event (e.g. click)
#define IDC_CLOSE_BUTTON 110
#define ID_HEALTH_BUTTON 111
#define ID_MONEY_BUTTON 112
#define ID_MANA_BUTTON 113
#define ID_SKULLS_BUTTON 114
#define ID_GAME_CONNECTION_STATE 115
#define IDT_TIMER1 116

#define WM_SETFONT 0x0030 // Sets the font that a control is to use when drawing text.
#define WM_SETTEXT 0x000C // Use this to change the text on the control
#define WM_CTLCOLORSTATIC 0x0138 // set the background colour of controls

// global variables
HWND Mainhwnd; // handle for the main window
HBITMAP bmpMainBackground = NULL;  // holds the image that will be used as the main window's background
HWND hBtnClose; // exit button's handle
HBITMAP bmpExitBtnBackground = NULL;  // holds the image that will be used as the main window's background
HWND hTitleText; // Health
HWND hHackHeadingText; // Health, Money, Skulls, Mana
HWND hBtnHealth; // Health button's handle
HWND hBtnMoney; // exit button's handle
HWND hBtnMana; // Health button's handle
HWND hBtnSkulls; // exit button's handle
HWND hBtnGameConnectionState; // handle for the control that shows the game state (connected or not)
HBITMAP bmpOnBtnBackground = NULL;  // holds the image that will be used as the main window's background
HBITMAP bmpOffBtnBackground = NULL;  // holds the image that will be used as the main window's background
HBITMAP bmpGameConnected = NULL;  // holds the image that will be used to show that the trainer is connected to the game
HBITMAP bmpNotGameConnected = NULL;  // holds the image that will be used to show that the trainer is not connected to the game

// Font definitions
HFONT fontCourierNew = CreateFont(-17, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
	DEFAULT_QUALITY, FF_DONTCARE, "Courier New");
// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd183499(v=vs.85).aspx
HFONT fontTimesNewRoman = CreateFont(36, 20, -300, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
HFONT fontImpact = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
HFONT fontArial = CreateFont(40, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));
HFONT fontArial2 = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));

// Colour definitions
// for RGB values: https://www.rapidtables.com/web/color/RGB_Color.html
HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hBrushLightGrey = CreateSolidBrush(RGB(200, 200, 200));
HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
HBRUSH hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
HBRUSH hBrushPink = CreateSolidBrush(RGB(255, 0, 255));
HBRUSH hBrushPurple = CreateSolidBrush(RGB(102, 0, 102));
HBRUSH hBrushMauve = CreateSolidBrush(RGB(76, 0, 153));
HBRUSH hBrushYellow = CreateSolidBrush(RGB(255, 255, 51));
HBRUSH hBrushOrange = CreateSolidBrush(RGB(255, 128, 0));
HBRUSH hBrushLightBlue = CreateSolidBrush(RGB(0, 128, 255));

									 // variables for the memory hack
									 // =============================
std::string GameName = "Orcs Must Die 2";
char ProcessName[] = "OrcsMustDie2.exe";
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

DWORD baseAddressOfProcess;

// Health related variables
bool HealthStatus;
FLOAT HealthValue = 0xC8; // 100 dec
DWORD HealthBaseAddress = { 0xBA4454 };
DWORD HealthOffsets[] = { 0x20, 0x18, 0x54 }; // 3 level pointer "OrcsMustDie2.exe"+007A4454
											  // Money related variables
bool bMoneyButton = false;
bool MoneyStatus = false;
BYTE MoneyValue[] = { 0x0, 0xFA, 0x0, 0x0 }; // 4000dec (FA0 in calculator)
DWORD MoneyBaseAddress = { 0xBA4454 }; // "OrcsMustDie2.exe"+007A4454 (+400000)
DWORD MoneyOffsets[] = { 0x20, 0x18, 0x7C }; // 3 level pointer
											 // Mana related variables
bool ManaStatus;
FLOAT ManaValue = 0x64; // 100 dec
DWORD ManaBaseAddress = { 0xBA4454 };
DWORD ManaOffsets[] = { 0x20, 0x18, 0x5C }; // 3 level pointer
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
DWORD FindDMA(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress)
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
		DWORD HealthAddressToWrite = FindDMA(3, hProcHandle, HealthOffsets, HealthBaseAddress);
		WriteProcessMemory(hProcHandle, (BYTE*)HealthAddressToWrite, &HealthValue, sizeof(FLOAT), NULL);
	}
	if (moneyBTnStateIsOn)
	{
		DWORD MoneyAddressToWrite = FindDMA(3, hProcHandle, MoneyOffsets, MoneyBaseAddress);
		if (!WriteProcessMemory(hProcHandle, (BYTE*)MoneyAddressToWrite, &MoneyValue, sizeof(MoneyValue), NULL));
			MessageBox(NULL, "Failed to hack money.", "Error!", MB_OK | MB_ICONERROR);
	}
	if (manaBTnStateIsOn)
	{
		DWORD ManaAddressToWrite = FindDMA(3, hProcHandle, ManaOffsets, ManaBaseAddress);
		WriteProcessMemory(hProcHandle, (BYTE*)ManaAddressToWrite, &ManaValue, sizeof(FLOAT), NULL);
	}
	/* Not implemented!
	if (skullsBTnStateIsOn)
	{
		// note, the call to FindDmaAddress() is not required, because there are no offsets to deal with
		DWORD SkullsAddressToWrite = FindDmaAddy(1, hProcHandle, SkullsOffsets, SkullsBaseAddress);
		WriteProcessMemory(hProcHandle, (BYTE*)SkullsBaseAddress, &SkullsValue, sizeof(SkullsValue), NULL);
	} 
	*/
} // END OF WriteToMemory(HANDLE hProcHandle)


/*
    This function checks to see if the game we wish to hack is running
*/
bool CheckIfGameIsAvailable(HWND hwnd)
{
	DWORD dwProcId = NULL;

	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	HANDLE hProcSnap;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hProcSnap, &pe32))
	{
		do
		{
			if (strcmp(pe32.szExeFile, ProcessName) == 0)
			{
				dwProcId = pe32.th32ProcessID; // the process ID of the game's process
			    // Get process Handle
				hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
				if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL)
				{
					// failed
					gameConnected = false;
					return false;
				}
				else
				{
					// trainer is connected to the game, show this in the Trainer window
					gameConnected = true;
					SendMessage(hBtnGameConnectionState, WM_SETTEXT, 0, (LPARAM)"Connected!"); // change the text to ON
					return true;
				}
				return true;
			}
		} while (Process32Next(hProcSnap, &pe32));
	}
	return false;
}// END OF checkIfGameIsAvailable(HWND hwnd)

  // this method executes the required hack
void applyHacks(HWND hwnd)
{
	IsGameAvail = CheckIfGameIsAvailable(hwnd);

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
	else
		SendMessage(hBtnGameConnectionState, WM_SETTEXT, 0, (LPARAM)"Not Connected!"); // change the text to OFF

} // END OF applyHacks(HWND hwnd)


  /*
  This function deals with all the messages sent to the GUI
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
			// Orcs Must Die 2 Title  			
			// =====================
			hTitleText = CreateWindowEx(NULL,
				"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
				"Orcs Must Die 2 Trainer",
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
				MessageBox(hwnd, "Could not create static text.", "Error!", MB_OK | MB_ICONERROR);
			// Set the font to Courier New
			SendMessage(hTitleText, WM_SETFONT, (WPARAM)fontArial, MAKELPARAM(true, 0));

			// Health heading  			
			// ==============
			hHackHeadingText = CreateWindowEx(NULL,
				"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
				"Health",
				WS_CHILD | WS_VISIBLE,
				420, // x
				110, // y
				123, // Width
				42, // Height
				hwnd,
				NULL,
				GetModuleHandle(NULL),
				NULL);
			if (hHackHeadingText == NULL)
				MessageBox(hwnd, "Could not create Health static text control.", "Error!", MB_OK | MB_ICONERROR);
			// Set the font to Courier New
			SendMessage(hHackHeadingText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

			// DEFINE A HEALTH BUTTON  
			// ======================
			hBtnHealth = CreateWindowEx(NULL,
				"BUTTON",
				"OFF",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
				500, // x
				100, // y
				123, // Width
				42,  // Height
				hwnd,
				(HMENU)ID_HEALTH_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnHealth == NULL)
				MessageBox(hwnd, "Could not create health button.", "Error", MB_OK | MB_ICONERROR);

			// Money heading  			
			// =============
			hHackHeadingText = CreateWindowEx(NULL,
				"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
				"Money",
				WS_CHILD | WS_VISIBLE,
				420, // x
				160, // y
				123, // Width
				42, // Height
				hwnd,
				NULL,
				GetModuleHandle(NULL),
				NULL);
			if (hHackHeadingText == NULL)
				MessageBox(hwnd, "Could not create Health static text control.", "Error!", MB_OK | MB_ICONERROR);
			// Set the font to Courier New
			SendMessage(hHackHeadingText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

			// DEFINE A MONEY BUTTON  
			// =====================
			hBtnMoney = CreateWindowEx(NULL,
				"BUTTON",
				"OFF",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
				500, // x
				150, // y
				123, // Width
				42,  // Height
				hwnd,
				(HMENU)ID_MONEY_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnMoney == NULL)
				MessageBox(hwnd, "Could not create money button.", "Error", MB_OK | MB_ICONERROR);

			// Mana heading  			
			// ============
			hHackHeadingText = CreateWindowEx(NULL,
				"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
				"Mana",
				WS_CHILD | WS_VISIBLE,
				420, // x
				210, // y
				123, // Width
				42, // Height
				hwnd,
				NULL,
				GetModuleHandle(NULL),
				NULL);
			if (hHackHeadingText == NULL)
				MessageBox(hwnd, "Could not create Health static text control.", "Error!", MB_OK | MB_ICONERROR);
			// Set the font to Courier New
			SendMessage(hHackHeadingText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

			// DEFINE A MANA BUTTON  
			// ====================
			hBtnMana = CreateWindowEx(NULL,
				"BUTTON",
				"OFF",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
				500, // x
				200, // y
				123, // Width
				42,  // Height
				hwnd,
				(HMENU)ID_MANA_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnMana == NULL)
				MessageBox(hwnd, "Could not create mana button.", "Error", MB_OK | MB_ICONERROR);

			// Skulls heading  			
			// =============
			hHackHeadingText = CreateWindowEx(NULL,
				"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
				"Skulls",
				WS_CHILD | WS_VISIBLE,
				420, // x
				260, // y
				123, // Width
				42, // Height
				hwnd,
				NULL,
				GetModuleHandle(NULL),
				NULL);
			if (hHackHeadingText == NULL)
				MessageBox(hwnd, "Could not create Health static text control.", "Error!", MB_OK | MB_ICONERROR);
			// Set the font to Courier New
			SendMessage(hHackHeadingText, WM_SETFONT, (WPARAM)fontArial2, MAKELPARAM(true, 0));

			// DEFINE A SKULLS BUTTON  
			// ======================
			hBtnSkulls = CreateWindowEx(NULL,
				"BUTTON",
				"OFF",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
				500, // x
				250, // y
				123, // Width
				42,  // Height
				hwnd,
				(HMENU)ID_SKULLS_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnSkulls == NULL)
				MessageBox(hwnd, "Could not create skulls button.", "Error", MB_OK | MB_ICONERROR);

			// DEFINE A BUTTON - EXIT program 
			// ==============================
			hBtnClose = CreateWindowEx(NULL,
				"BUTTON",
				"Exit",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
				520, // x
				320,  // y
				70, // Width
				30, // Height
				hwnd,
				(HMENU)IDC_CLOSE_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnClose == NULL)
				MessageBox(hwnd, "Could not create button.", "Error", MB_OK | MB_ICONERROR);

			// DEFINE THE GAME CONNECTION STATE CONTROL  
			// ========================================
			hBtnGameConnectionState = CreateWindowEx(NULL,
				"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
				"Not Connected",
				WS_CHILD | WS_VISIBLE,
				110, // x
				169, // y
				300, // Width
				42, // Height
				hwnd,
				(HMENU)ID_GAME_CONNECTION_STATE,
				GetModuleHandle(NULL),
				NULL);
			if (hBtnGameConnectionState == NULL)
				MessageBox(hwnd, "Could not create Game Connection State control.", "Error!", MB_OK | MB_ICONERROR);
			// Set the font to Courier New
			SendMessage(hBtnGameConnectionState, WM_SETFONT, (WPARAM)fontArial, MAKELPARAM(true, 0));			

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

				if (healthBTnStateIsOn)
				{
					SendMessage(hBtnHealth, WM_SETTEXT, 0, (LPARAM)"ON"); // change the text to ON
				}
				else
					SendMessage(hBtnHealth, WM_SETTEXT, 0, (LPARAM)"OFF"); // change the text to OFF

				InvalidateRect(hBtnHealth, 0, 1); // FORCE the redrawing of the Money button
			}
			break;
			case ID_MONEY_BUTTON: // switch the Money button between On and Off
			{
				// set the state of the button
				moneyBTnStateIsOn = !moneyBTnStateIsOn;

				if (moneyBTnStateIsOn)
				{
					SendMessage(hBtnMoney, WM_SETTEXT, 0, (LPARAM)"ON"); // change the text to ON
				}
				else
					SendMessage(hBtnMoney, WM_SETTEXT, 0, (LPARAM)"OFF"); // change the text to OFF

				InvalidateRect(hBtnMoney, 0, 1); // FORCE the redrawing of the Money button
			}
			break;
			case ID_MANA_BUTTON: // switch the Mana button between On and Off
			{
				// set the state of the button
				manaBTnStateIsOn = !manaBTnStateIsOn;

				if (manaBTnStateIsOn)
				{
					SendMessage(hBtnMana, WM_SETTEXT, 0, (LPARAM)"ON"); // change the text to ON
				}
				else
					SendMessage(hBtnMana, WM_SETTEXT, 0, (LPARAM)"OFF"); // change the text to OFF

				InvalidateRect(hBtnMana, 0, 1); // FORCE the redrawing of the Mana button
			}
			break;
			case ID_SKULLS_BUTTON: // switch the Skulls button between On and Off
			{
				// set the state of the button
				skullsBTnStateIsOn = !skullsBTnStateIsOn;

				if (skullsBTnStateIsOn)
				{
					SendMessage(hBtnSkulls, WM_SETTEXT, 0, (LPARAM)"ON"); // change the text to ON
				}
				else
					SendMessage(hBtnSkulls, WM_SETTEXT, 0, (LPARAM)"OFF"); // change the text to OFF

				InvalidateRect(hBtnSkulls, 0, 1); // FORCE the redrawing of the Skulls button
			}
			break;
	    }
	    break;
		case WM_CTLCOLORSTATIC: // need this to change/set the background colour of the static controls (e.g. title, connection status)
		{
			HDC hdcStatic = (HDC)wParam;
			//			SetTextColor(hdcStatic, RGB(200, 200, 200));
			SetBkColor(hdcStatic, RGB(200, 200, 200));
			return (INT_PTR)CreateSolidBrush(RGB(200, 200, 200));

		}
		case WM_DESTROY: // REQUIRED for killing the program's process
			PostQuitMessage(0);
			break;
		case WM_NCHITTEST: // Make the client area of the main window draggable
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