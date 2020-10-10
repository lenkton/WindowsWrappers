#include <Windows.h>
#include <string>
#include "GDIWrappers.h"

std::unordered_map<HWND, antonov::WNDWRAPPER*> antonov::WNDWRAPPER::pointers 
	= std::unordered_map<HWND, antonov::WNDWRAPPER*>();

LRESULT(*antonov::WNDWRAPPER::defProc)(HWND, UINT, WPARAM, LPARAM)
	=DefWindowProc;
