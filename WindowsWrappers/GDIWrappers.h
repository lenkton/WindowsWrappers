#pragma once
#include <Windows.h>
#include <string>
#include<unordered_map>
#include <list>
#include <typeinfo>




namespace antonov {

	class HDCWrapper {
	private:
	protected: HDC hdc;
	public:
		class _context {
		private: HDCWrapper* parent;
				 void selAndDel(HGDIOBJ h, bool deleteCond) {
					 if (deleteCond)
						 DeleteObject(SelectObject(parent->hdc, h));
					 else
						 SelectObject(parent->hdc, h);
				 }
		public:
			_context(HDCWrapper* p = nullptr) : parent(p) {}
			void setTextColor(COLORREF color) { SetTextColor(parent->hdc, color); }
			bool hasDeletablePen = false;
			void setPen(HPEN&& hpen) {
				selAndDel(hpen, hasDeletablePen);
				hasDeletablePen = true;
			}
			void setPen(HPEN& hpen) {
				selAndDel(hpen, hasDeletablePen);
				hasDeletablePen = false;
			}

			void setBrushDO(HBRUSH hbrush) {
				DeleteObject(SelectObject(*parent, hbrush));
			}
			void setFontDO(HFONT hfont) {
				DeleteObject(SelectObject(*parent, hfont));
			}
			~_context() {
				selAndDel(GetStockObject(BLACK_PEN), hasDeletablePen);
				DeleteObject(SelectObject(*parent, GetStockObject(BLACK_BRUSH)));
				DeleteObject(SelectObject(*parent, GetStockObject(SYSTEM_FONT)));
			}
		} context;
		void inline setViewportOrigin(int x, int y) {
			SetViewportOrgEx(hdc, x, y, NULL);
		}
		HDCWrapper(HDC _hdc) {
			hdc = _hdc;
			context = _context(this);
		}
		HDCWrapper(const HDCWrapper& h) = delete;
		HDCWrapper& operator=(const HDCWrapper& h) = delete;
		/*HDCWrapper(HDCWrapper&& h) {
			hdc = h.hdc;
			h.hdc = 0;
		}*/
		HDCWrapper& operator=(HDCWrapper&& h) {
			this->hdc = h.hdc;
			h.hdc = 0;
			this->context = h.context;
			h.context.~_context();
		}


		//maybe needs further development
		//for example, context is being copied,
		//but being moved is preferable
		HDCWrapper(HDCWrapper&& h)  {
			this->hdc = h.hdc;
			h.hdc = 0;
			this->context = h.context;
			h.context.~_context();

		}

		operator HDC() const {
			return hdc;
		}
		
		virtual ~HDCWrapper() {}
		void inline printString(int x, int y, std::wstring s) {
			TextOut(hdc, x, y, s.c_str(), s.length());
		}

	};

	class PaintHDC : public HDCWrapper {
	private:
		PAINTSTRUCT ps;
		HWND hwnd;
	public:

		PaintHDC(HWND _hwnd)
			:HDCWrapper(BeginPaint(_hwnd, &ps)),
			hwnd(_hwnd)
		{}
		PaintHDC(PaintHDC&& p): HDCWrapper(std::move(p))  {
			this->ps = p.ps;
			p.ps = {};
			this->hwnd = p.hwnd;
			p.hwnd = 0;
		}
		~PaintHDC() {
			if(hdc)
				EndPaint(hwnd, &ps);
		}
	};

	class GetDCWrapper :public HDCWrapper {
	private:
		HWND hwnd;
	public:
		GetDCWrapper(GetDCWrapper&& g): HDCWrapper(std::move(g)) {
			this->hdc = g.hdc;
			g.hdc = 0;
		}
		GetDCWrapper operator=(GetDCWrapper&& g) {
			this->hdc = g.hdc;
			g.hdc = 0;
		}
		GetDCWrapper(HWND _hwnd)
			:HDCWrapper(GetDC(_hwnd)),
			hwnd(_hwnd)
		{}
		~GetDCWrapper() {
			if(hdc)
				ReleaseDC(hwnd, hdc);
		}
	};

	class BitmapWrapper {
	private: BITMAP bitmap;
	};

	class CompatDCWrapper :public HDCWrapper {
	public:
		CompatDCWrapper(HDC _hdc)
			:HDCWrapper(CreateCompatibleDC(_hdc))
		{}
		~CompatDCWrapper() {
			if(hdc)DeleteDC(hdc);
		}
	};


	//You need to Derive from this class,
	//declare static std::list<WNDWRAPPER> instances.
	//static LRESULT (*defProc)(HWND,UINT, WPARAM,LPARAM),
	//static ATOM reg(HINSTANCE _hInstance)
	//	where you need to assign wndProc to 
	//	antonov::WNDWRAPPER::wndProcAdapter<YourClassName>
	//and override nesessary messages
	class WNDWRAPPER {
	private: 
		
		static std::unordered_map<HWND, WNDWRAPPER*> pointers;
		//static std::list<WNDWRAPPER> instances;

		HWND hwnd;
		static LRESULT (*defProc)(HWND,UINT, WPARAM,LPARAM);

		
		//static bool isRegistred;
		//static WNDCLASSEX wndClassEx;
		
	protected:
		WNDWRAPPER() :hwnd(0) {}
		WNDWRAPPER(HWND _hwnd) :hwnd(_hwnd) {}
		virtual ~WNDWRAPPER(){}
		virtual LRESULT destroy(WPARAM wParam, LPARAM lParam) {
			/*PostQuitMessage(0);
			return 0;*/
			return this->defProc(*this, WM_DESTROY, wParam, lParam);
		}
		virtual LRESULT paint(WPARAM wParam, LPARAM lParam) {
			return this->defProc(*this, WM_PAINT, wParam, lParam);
		}
		virtual LRESULT create(CREATESTRUCT* pcs) {
			return this->defProc(*this, WM_CREATE, 0, (LPARAM)pcs);
			//return DefWindowProc(hwnd, WM_CREATE, 0, (LPARAM)pcs);
		}
		virtual LRESULT size(WPARAM wParam, LPARAM lParam) {
			return this->defProc(*this, WM_SIZE, wParam, lParam);
		}
		virtual LRESULT mouseMove(WPARAM wParam, LPARAM lParam) {
			return this->defProc(*this, WM_MOUSEMOVE, wParam, lParam);
		}
		
		
	public:
		antonov::GetDCWrapper getDC() {
			return antonov::GetDCWrapper(hwnd);
		}
		antonov::PaintHDC startPaint() {
			return antonov::PaintHDC(hwnd);
		}
		static ATOM reg(HINSTANCE _hInstance) {};

		template <typename T>
		static LRESULT CALLBACK wndProcAdapter(
			HWND _hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
			switch (iMsg)
			{
			case WM_CREATE:
				pointers[_hwnd] = new T{ _hwnd };
				//createT<T>(_hwnd, (LPCREATESTRUCT)lParam);
				return pointers[_hwnd]->create((LPCREATESTRUCT)lParam);
			case WM_MOUSEMOVE:
				return pointers[_hwnd]->mouseMove(wParam, lParam);
			case WM_SIZE:
				return pointers[_hwnd]->size(wParam, lParam);
			case WM_PAINT:
				return pointers[_hwnd]->paint(wParam, lParam);
			case WM_DESTROY:
				auto a = pointers[_hwnd]->destroy(wParam, lParam);
				delete pointers[_hwnd];
				return a;
			}
			return T::defProc(_hwnd, iMsg, wParam, lParam);
		}
		
		operator HWND() {
			return hwnd;
		}
		/*WNDWRAPPER(std::wstring = std::wstring(L"Noname")) {
			if (!isRegistred) {
				throw std::exception::exception("The class has not been "
				"registered yet!");
			}
			hwnd = CreateWindow(wndClassEx.lpszClassName,ws.c_str(),
				)
		}*/
		
		
		/*static int reg(HINSTANCE hInstance, std::wstring ws) {
			isRegistred = true;
			
			wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
			wndClassEx.lpfnWndProc = wndProcAdapter;
			wndClassEx.cbClsExtra = 0;
			wndClassEx.cbWndExtra = 0;
			wndClassEx.hInstance = hInstance;
			wndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wndClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndClassEx.lpszMenuName = NULL;
			wndClassEx.lpszClassName = ws.c_str();
			wndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			return RegisterClassEx(&wndClassEx);
	}*/
		
	};
	
	}