// DotaItemMover.cpp : Defines the entry point for the console application.
//

#include "CImg.h"
#include<windows.h>
#include<iostream>
#include<thread>
#include<map>
#include<vector>
#include<fstream>
#include<sstream>
#pragma comment(lib, "user32.lib")
using namespace std;
/* Globals */
int ScreenX = GetDeviceCaps(GetDC(0), HORZRES);
int ScreenY = GetDeviceCaps(GetDC(0), VERTRES);
vector<BYTE> ScreenData(3 * ScreenX*ScreenY);
bool isPicking = false;
int* referenceDragPoint;
void pick();

void ScreenCap() {
	HDC hdc = GetDC(GetDesktopWindow());
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, ScreenX, ScreenY);
	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 24;
	bmi.biWidth = ScreenX;
	bmi.biHeight = -ScreenY;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = ScreenX * ScreenY;
	SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hdc, 0, 0, SRCCOPY);
	GetDIBits(hdc, hBitmap, 0, ScreenY, &ScreenData[0], (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdc);
	DeleteObject(hBitmap);
}

inline int PosR(int x, int y) {
	return ScreenData[3 * ((y*ScreenX) + x) + 2];
}

inline int PosG(int x, int y) {
	return ScreenData[3 * ((y*ScreenX) + x) + 1];
}

inline int PosB(int x, int y) {
	return ScreenData[3 * ((y*ScreenX) + x)];
}

inline int* colorAt(int x, int y) {
	int* color = new int[3]{ PosR(x,y), PosG(x,y), PosB(x,y) };
	return color;
}

inline tuple<BYTE, BYTE, BYTE> colorAt2(int x, int y) {
	return{ PosR(x,y),PosG(x,y),PosB(x,y) };
}

void clickAt(int x, int y) {
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	SetCursorPos(x, y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	SetCursorPos(cursorPos.x, cursorPos.y);
}

bool colorApprox(tuple<BYTE, BYTE, BYTE> t1, tuple<BYTE, BYTE, BYTE> t2) {
	return abs(((int)get<0>(t1)) - ((int)get<0>(t2))) < 45
		&& abs(((int)get<1>(t1)) - ((int)get<1>(t2))) < 45
		&& abs(((int)get<2>(t1)) - ((int)get<2>(t2))) < 45;
}

void printTuple(tuple<BYTE, BYTE, BYTE> t) {
	cout << (int)get<0>(t) << ',' << (int)get<1>(t) << ',' << (int)get<2>(t);
}


int NONE = 0;
int MONKEY = 1;
int selected_hero = NONE;

map<int, int*> HERO_POS = {
	{ MONKEY, new int[2] { 1112,415 } }
};

void pickHero() {
	int* pickPoint = new int[2]{ 1420, 794 };//the pick button
	if (selected_hero == MONKEY) {
		int* monkeyPoint = HERO_POS.at(MONKEY);
		clickAt(monkeyPoint[0], monkeyPoint[1]);//select hero
		Sleep(80);
		clickAt(pickPoint[0], pickPoint[1]);//confirm
		Sleep(80);
	}
}

void beep(int times, int interval) {
	for (size_t i = 0; i < times; i++)
	{
		cout << '\a';
		Sleep(interval);
	}
}

void tryPickMid() {
	int tries = 0;
	while (isPicking) {
		time_t time1 = time(0);
		ScreenCap();
		time_t time2 = time(0);
		cout << time2 - time1 << endl;
		auto direColor = colorAt2(298, 943);
		auto radiantColor = colorAt2(253, 965);
		auto goColor = make_tuple(12, 213, 27);
		cout << "direColor = ";
		printTuple(direColor);
		cout << endl;
		cout << "radiantColor = ";
		printTuple(radiantColor);
		cout << endl;
		cout << "expected = ";
		printTuple(goColor);
		cout << endl;
		if (colorApprox(direColor, goColor)) {
			clickAt(298, 943);
			isPicking = false;
			beep(2, 100);
		}
		else if (colorApprox(radiantColor, goColor)) {
			clickAt(253, 965);
			isPicking = false;
			beep(2, 100);
		}
		else if (tries++ == 1000) {
			isPicking = false;
			beep(3, 200);
		}
	}
	/*hero selection (it is assured that we are past
	lane pick so we can choose hero)*/
	Sleep(10);
	pickHero();
}

long double last_item_change_time = 0;
map<int, int*> SLOT_POS = {
	/*
	OLD TIMES   (NOICE)
	{ 4, new int[2] { 1499,1020} },
	{ 5, new int[2] { 1579,1020} },
	{ 6, new int[2] { 1657,1020} },
	{ 7, new int[2] { 1499,963 } },
	{ 8, new int[2] { 1579,963 } },
	{ 9, new int[2] { 1657,963 } }
			_,--,
		 .-'---./_    __
		/o \\     "-.' /
		\  //    _.-'._\
		 `"\)--"`			     */
		 /*{ 1, new int[2]{ 1210,1065 } },
		 { 2, new int[2]{ 1271,1065 } },
		 { 3, new int[2]{ 1333,1065 } },
		 { 4, new int[2]{ 1205,1021 } },
		 { 5, new int[2]{ 1267,1019 } },
		 { 6, new int[2]{ 1324,1012 } },
		 { 7, new int[2]{ 1213,976 } },
		 { 8, new int[2]{ 1268,978 } },
		 { 9, new int[2]{ 1326,977 } } < -RECENT SHITTY TIMES*/

		 // << U P D A T E D >>
		 { 4, new int[2]{   -2, -44} },
		 { 5, new int[2]{  +62, -44} },
		 { 6, new int[2]{ +145, -44} },
		 { 7, new int[2]{   -2, -90} },
		 { 8, new int[2]{  +70, -90} },
		 { 9, new int[2]{ +145, -90} }   /* these are the offsets in relation to the tiny sword pointing to total physical damage*/
};

int from = -1, to = -1;
void exec(int times) {
	int* slotPos1 = SLOT_POS.at(from);
	int* slotPos2 = SLOT_POS.at(to);
	POINT saved;
	GetCursorPos(&saved);
	SetCursorPos(referenceDragPoint[0] + slotPos1[0], referenceDragPoint[1] + slotPos1[1]);
	Sleep(40);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(30);
	SetCursorPos((referenceDragPoint[0] * 2 + slotPos1[0] + slotPos2[0]) / 2, (referenceDragPoint[1] * 2 + slotPos1[1] + slotPos2[1]) / 2);
	Sleep(50);
	SetCursorPos(referenceDragPoint[0] + slotPos2[0], referenceDragPoint[1] + slotPos2[1]);
	Sleep(30);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	SetCursorPos(saved.x, saved.y);
}

void findReferencePointForDrag() {
	ScreenCap();
	for (int i = 1068/*x loc for base template*/; i < ScreenX; i++) {
		if (colorApprox(colorAt2(i, 1064), make_tuple(127, 127, 127)/*color of template*/)) {
			referenceDragPoint = new int[2]{ i, 1064 };
			return;
		}
	}
	referenceDragPoint = new int[2]{ -1, -1 };
}

void addToStack(int slot) {
	if (time(0) - last_item_change_time > 60 * 3/*3 mins*/) {
		findReferencePointForDrag();
		cout << "Reference Drag Point:" << referenceDragPoint[0] << ',' << referenceDragPoint[1] << endl;
	}
	cout << "added to stack: " + slot << endl;
	if (from == -1 && to == -1) {
		from = slot;
		cout << "from=" << from << endl;;
	}
	else if (from != -1 && to == -1) {
		to = slot;
		cout << "to=" << to << endl;;
		if (from != to) {
			exec(2);
		}
		cout << "exec'd" << endl;
		from = to = -1;
	}
	else if (from != -1 && to != -1) {
		to = -1;
		from = slot;
	}
}

void selectHero() {
	cin.sync();
	cin.clear();
	cout << endl << "Input hero number:" << endl;
	cout << "0 - None" << endl;
	cout << "1 - Monkey King" << endl;
	char a;
	cin >> a;
	selected_hero = a - '0';
}

HHOOK hHook;
thread t;
void pick() {
	if (!t.joinable()) {
		isPicking = true;
		cout << '\a';
		t = thread(tryPickMid);
	}
	else {
		isPicking = false;
		t.join();
		pick();//idk why
	}
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
		cout << pKey->vkCode << " ";
		switch (pKey->vkCode) {
		case 97://1
			addToStack(1);
			break;
		case 98://2
			addToStack(2);
			break;
		case 99://3
			addToStack(3);
			break;
		case 100://4
			addToStack(4);
			break;
		case 101://5
			addToStack(5);
			break;
		case 102://6
			addToStack(6);
			break;
		case 103://7
			addToStack(7);
			break;
		case 104://8
			addToStack(8);
			break;
		case 105://9
			addToStack(9);
			break;
		case 106: {//*
			POINT saved;
			GetCursorPos(&saved);
			ScreenCap();
			int* color = colorAt(saved.x, saved.y);
			cout << saved.x << ',' << saved.y << '=' << color[0] << ',' << color[1] << ',' << color[2] << endl;
			delete[] color;
			break;
		}
		case 110://del (try pick mid)
			pick();
			break;
		case 111:///
			selectHero();
			break;
		}
	}
	CallNextHookEx(hHook, nCode, wParam, lParam);
	return 0;
}

void startLogging() {
	RegisterHotKey(NULL, 0xB1AC7B1A, MOD_ALT, VK_F12);
	HMODULE hInstance = GetModuleHandle(NULL);
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL);
	MSG msg;
	GetMessage(&msg, NULL, NULL, NULL);
	UnhookWindowsHookEx(hHook);
}

int main() {
	cout << "Runnning..." << endl;
	if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) {//if numlock not enabled
		keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY, 0);//enable
		keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
	startLogging();
	return 0;
}

