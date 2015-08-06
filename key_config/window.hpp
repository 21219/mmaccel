#pragma once

#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/gui/dropdown_box.hpp>
#include <mmaccel/gui/list_view.hpp>
#include <mmaccel/gui/tab.hpp>
#include "resource.h"

namespace mmaccel { namespace key_config
{
	class window_impl
	{
		HWND wnd_;
		dropdown_box< IDC_SELECTOR > selector_;
		tabs< IDC_TAB > tabs_;
		list_view< IDC_LIST_VIEW > list_view_;

		window_impl():
			wnd_( winapi::modeless_dialog_box( IDD_KEY_CONFIG, nullptr, &proc ) ),
			selector_( wnd_ )
		{ 
		}

	public:
		void show() const noexcept
		{
			ShowWindow( wnd_, SW_SHOW );
		}

		static window_impl& instance()
		{
			static std::unique_ptr< window_impl > obj( new window_impl );
			return *obj;
		}

	private:
		void on_command(int id, int, HWND)
		{
			if( id == IDOK ) {
				PostMessageW( wnd_, WM_CLOSE, 0, 0 );
			}
			else if( id == IDCANCEL ) {
				PostMessageW( wnd_, WM_CLOSE, 0, 0 );
			}
		}

		static INT_PTR CALLBACK proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
		{
			if( msg == WM_CLOSE ) {
				DestroyWindow( hwnd );
				return TRUE;
			}
			if( msg == WM_DESTROY ) {
				PostQuitMessage( 0 );
				return TRUE;
			}

			if( msg == WM_COMMAND ) {
				instance().on_command( LOWORD( wparam ), HIWORD( wparam ), reinterpret_cast<HWND>( lparam ) );
			}

			return FALSE;
		}
	};

	inline window_impl& window()
	{
		return window_impl::instance();
	}

} } // namespace mmaccel::key_config