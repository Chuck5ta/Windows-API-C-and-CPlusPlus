/*
This is an example of the creation of the main window for a Win32 application
*/
#include <Windows.h>
#include "commctrl.h" // required for WC_COMBOBOX wimdows class
// Windows classes: https://msdn.microsoft.com/en-us/library/windows/desktop/bb775491(v=vs.85).aspx
// Required for hacking
#include <TlHelp32.h> // for searching for processes (PROCESSENTRY32)
#include <psapi.h> // required for EnumProcessModules and GetModuleFileNameEx (to acquire base address)
#include <string>
#include <sstream> // conversion from DWORD to string

#include "DataConversion.h"

// these are used to ID the controls/windows, and are used to execute an event (e.g. click)
#define IDC_CLOSE_BUTTON 110
#define ID_GET_BASE_ADDRESS_BUTTON 111
#define ID_EDITCHILD 112 // The edit control (need one of these for each edit control)
#define ID_COMBOBOX 113

// global variables
HWND Mainhwnd; // handle for the main window
HWND hWndComboBox; // handle for the combobox
HWND hBtnGetBaseAddress;
HWND hEditControlHeading;
HWND hEdit;
HWND hTitleText; // handle for the label
HWND hComboBoxHeading; 

// Colour definitions
// for RGB values: https://www.rapidtables.com/web/color/RGB_Color.html
HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hBrushLightGrey = CreateSolidBrush(RGB(200, 200, 200));

// Font definitions
// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd183499(v=vs.85).aspx
HFONT fontComicSans = CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Comic Sans MS"));
HFONT fontArial = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));

// variables for the memory hack
// =============================
//std::string GameName = "Orcs Must Die 2";
char ProcessName[] = "OrcsMustDie2.exe";
// Process related variables
DWORD dwProcId = NULL; // the process ID 
HANDLE hProcHandle = NULL; // process handle
DWORD dwBaseAddress = NULL;

// these hold the names and corresponding process IDs of the currently running processes
std::string aProcessName[1000]; // 1000 items of 260 in size
DWORD aBaseAddress[1000];
int iTotalProcesses;

/*
This function acquires the base address of the process's main exe
*/
DWORD GetBaseAddress(HWND hwnd)
{
	HMODULE hMods[1024];
	DWORD dwNeeded;
	// Get a list of all the modules in this process.
	if (EnumProcessModules(hProcHandle, hMods, sizeof(hMods), &dwNeeded))
	{
		for (int i = 0; i < (dwNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				std::string sModuleName = szModName;

				int found = sModuleName.find_last_of("\\");
				std::string str3 = sModuleName.substr(found + 1);

				if (str3.compare(ProcessName) == 0)
				{
					DWORD address = (DWORD)hMods[i];
					std::string streeeee = ConvertDWORDToString(address);
					return (DWORD)hMods[i];
				}
			}
		}
	}
	SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"Failed to connect!");
	return NULL;
}

/*
This function checks to see if the game we wish to hack is running
*/
bool CheckIfGameIsAvailable(HWND hwnd)
{
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
					SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"ERROR: Cannot connect to game!");
					return false;
				}
				else
				{
					// trainer is connected to the game, show this in the Trainer window
					SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"Connected!"); // change the text to ON
					dwBaseAddress = GetBaseAddress(hwnd);
					if (dwBaseAddress != NULL)
					{
						SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)ConvertDWORDToString(dwBaseAddress).c_str());
					}
					else
						SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"Failed to acquire base address!");

					return true;
				}
			}
		} while (Process32Next(hProcSnap, &pe32));

		MessageBox(hwnd, "Could not find game.", "Error", MB_OK | MB_ICONERROR);
	}
	return false;
}// END OF checkIfGameIsAvailable(HWND hwnd)

/*
    This sorts the processes into alphabetical order
*/
void SortProcessesIntoAphabeticalOrder()
{
	DWORD dwTemp;
	std::string sTemp;

	for (int i = 0; i < iTotalProcesses; ++i)
	{
		for (int j = i+1; j < iTotalProcesses; ++j)
		{
			if (aProcessName[j] < aProcessName[i])
			{
				// swap names
				sTemp = aProcessName[i];
				aProcessName[i] = aProcessName[j];
				aProcessName[j] = sTemp;
				// swap IDs
				dwTemp = aBaseAddress[i];
				aBaseAddress[i] = aBaseAddress[j];
				aBaseAddress[j] = dwTemp;
			}
		}
	}
}

/*
   This fills the combobox with the process names
*/
void FillComboBox()
{
	for (int i = 0; i < iTotalProcesses; i++)
	{
	    SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)aProcessName[i].c_str());
	}
}

/*
    This function checks to see if the game we wish to hack is running
*/
void GetListOfProcesses(HWND hwnd)
{
	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	HANDLE hProcSnap;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	int iProcessCount = 0;
	if (Process32First(hProcSnap, &pe32))
	{
		do
		{
			if (pe32.th32ProcessID != NULL || pe32.szExeFile == "")
			{
				dwProcId = pe32.th32ProcessID; // the process ID of the game's process
											   // Get process Handle
				hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);

				dwBaseAddress = GetBaseAddress(hwnd);

				// need to store the process name and ID in arrays
				aProcessName[iProcessCount] = pe32.szExeFile;
				aBaseAddress[iProcessCount] = dwBaseAddress;
				iProcessCount++;
			}
		} while (Process32Next(hProcSnap, &pe32));
	}
	// sort processes into alphebetical order
	iTotalProcesses = iProcessCount;
	SortProcessesIntoAphabeticalOrder();
	FillComboBox();
	SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)ConvertDWORDToString(iTotalProcesses).c_str());
	// set the 1st process in the combobox to be drisplayed by default
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

}// END OF checkIfGameIsAvailable(HWND hwnd)

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
			"Get Base Address Tool",
			WS_CHILD | WS_VISIBLE,
			180, // x
			30, // y
			370, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hTitleText == NULL)
			MessageBox(hwnd, "Could not create static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Comic Sans
		SendMessage(hTitleText, WM_SETFONT, (WPARAM)fontComicSans, MAKELPARAM(true, 0));

		// Define a text control (Static control)  			
		// ======================================
		hComboBoxHeading = CreateWindowEx(NULL,
			"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
			"Select process",
			WS_CHILD | WS_VISIBLE,
			410, // x
			160, // y
			370, // Width
			40, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hComboBoxHeading == NULL)
			MessageBox(hwnd, "Could not create ComboBox heading!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Arial
		SendMessage(hComboBoxHeading, WM_SETFONT, (WPARAM)fontArial, MAKELPARAM(true, 0));

		// Define an ComboBox control 			
		// ==========================
		hWndComboBox = CreateWindow(
			WC_COMBOBOX,
			TEXT(""),
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
			400, // x
			180, // y
			200, // width
			400, // height of drop down window area (scroll contents with arrow keys)
			hwnd,
			(HMENU)ID_COMBOBOX,
			GetModuleHandle(NULL),
			NULL); 

		// DEFINE A GET BASE ADDRESS BUTTON  
		// ================================
		hBtnGetBaseAddress = CreateWindowEx(NULL,
			"BUTTON",
			"Get base address",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Push button appearance
			260, // x
			290, // y
			123, // Width
			42,  // Height
			hwnd,
			(HMENU)ID_GET_BASE_ADDRESS_BUTTON,
			GetModuleHandle(NULL),
			NULL);
		if (hBtnGetBaseAddress == NULL)
			MessageBox(hwnd, "Could not create get base address button!", "Error", MB_OK | MB_ICONERROR);

		// Define a text control (Static control)  			
		// ======================================
		hEditControlHeading = CreateWindowEx(NULL,
			"STATIC", // Static window class (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system)
			"Base Address",
			WS_CHILD | WS_VISIBLE,
			60, // x
			140, // y
			200, // Width
			20, // Height
			hwnd,
			NULL,
			GetModuleHandle(NULL),
			NULL);
		if (hEditControlHeading == NULL)
			MessageBox(hwnd, "Could not create static control!", "Error!", MB_OK | MB_ICONERROR);
		// Set the font to Comic Sans
		SendMessage(hEditControlHeading, WM_SETFONT, (WPARAM)fontArial, MAKELPARAM(true, 0));

		// Define an Edit control 			
		// ======================
		hEdit = CreateWindowEx(
			NULL, "EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			50,  // x
			160,  // y
			200, // Width
			60, // Height
			hwnd,         // parent window 
			(HMENU)ID_EDITCHILD,   // edit control ID
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);        // pointer not needed
		if (hEdit == NULL)
			MessageBox(hwnd, "Could not create edit control.", "Error", MB_OK | MB_ICONERROR);
		// Add text to the window. ;
		SendMessage(hEdit, WM_SETFONT, (WPARAM)fontComicSans, 0);
	}
	case WM_COMMAND:
		switch (wParam)
		{
			case ID_GET_BASE_ADDRESS_BUTTON: // switch the Health button between On and Off
			{
				// execute the get base address function
				GetListOfProcesses(hwnd);
			}
			break;
		}
		case ID_COMBOBOX:
			switch (HIWORD(wParam))
			{
				case CBN_SELCHANGE:
				{
					// get index of item
					int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

					// copy the process ID to the edit box
					SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)ConvertDWORDToString(aBaseAddress[ItemIndex]).c_str());
				}
			    break;
			}
       break;
	break;
	case WM_CTLCOLORSTATIC: // need this to change/set the background colour of the static controls (e.g. title, labels, etc)
	{   // this will change the text and background colour of all Static Control (text/labels)
		HDC hdcStatic = (HDC)wParam;
	//	SetTextColor(hdcStatic, RGB(255, 0, 0)); // Change the text colour to white
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
		"Get base address of executable",
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
