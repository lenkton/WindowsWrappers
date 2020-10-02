#include <Windows.h>
#include <string>
#include "GDIWrappers.h"

std::unordered_map<HWND, antonov::WNDWRAPPER*> antonov::WNDWRAPPER::pointers 
	= std::unordered_map<HWND, antonov::WNDWRAPPER*>();

//std::list<antonov::WNDWRAPPER> antonov::WNDWRAPPER::instances;
LRESULT(*antonov::WNDWRAPPER::defProc)(HWND, UINT, WPARAM, LPARAM)
	=DefWindowProc;

//LRESULT antonov::WNDWRAPPER::paint(WPARAM wParam,LPARAM lParam)

//bool antonov::WNDWRAPPER::isRegistred = false;
//namespace antonov {
//	class HDCWrapper {
//	private: bool hasCustomPen;
//			 HPEN  defPen;
//	protected: HDC hdc;
//	public:
//		void inline setViewportOrigin(int x, int y) {
//			SetViewportOrgEx(hdc, x, y, NULL);
//		}
//		HDCWrapper(HDC _hdc) {
//			hdc = _hdc;
//			hasCustomPen = false;
//			defPen = NULL;
//			//pen = NULL;
//		}
//		operator HDC() const {
//			return hdc;
//		}
//		
//		void setPen(COLORREF _color) {
//			if (!hasCustomPen) {
//				defPen =(HPEN) SelectObject(
//					hdc, CreatePen(PS_SOLID, 1, _color));
//				hasCustomPen = true;
//			}
//			else { 
//				DeleteObject(SelectObject(
//					hdc, CreatePen(PS_SOLID, 1, _color)));
//			}
//		}
//		~HDCWrapper() {
//			if (hasCustomPen) DeleteObject(SelectObject(hdc,defPen));
//		}
//		void inline printString(int x, int y, std::wstring s) {
//			TextOut(hdc, x, y, s.c_str(), s.length());
//		}
//	};
//
//	class PaintHDC : public HDCWrapper{
//	private:
//		PAINTSTRUCT ps;
//		HWND hwnd;
//	public :
//		
//		PaintHDC(HWND _hwnd)
//			:HDCWrapper(BeginPaint(_hwnd, &ps)),
//			hwnd ( _hwnd)
//		{}
//		~PaintHDC() {
//			EndPaint(hwnd, &ps);
//		}
//	};
//
//	class GetDCWrapper:public HDCWrapper {
//	private:
//		HWND hwnd;
//	public:
//		GetDCWrapper(HWND _hwnd)
//			:HDCWrapper(GetDC(_hwnd)),
//			hwnd(_hwnd)
//		{}
//		~GetDCWrapper() {
//			ReleaseDC(hwnd, hdc);
//		}
//	};
//	
//	class BitmapWrapper {
//	private: BITMAP bitmap;
//	};
//
//	
//}

//LRESULT antonov::WNDWRAPPER::paint(WPARAM wParam, LPARAM lParam)
//{
//	antonov::PaintHDC paint(*this);
//	paint.printString(40, 40, L"poo");
//	return 0;
//}
