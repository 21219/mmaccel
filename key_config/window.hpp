#pragma once

#include <mmaccel/winapi/dialog.hpp>
#include "resource.h"

namespace mmaccel { namespace key_config
{
	class window
	{
		HWND wnd_;

	public:
		window():
			wnd_( winapi::modeless_dialog_box( nullptr, IDD_KEY_CONFIG, nullptr, &proc ) )
		{ }

		void show() const noexcept
		{
			ShowWindow( wnd_, SW_SHOW );
		}

	private:
		static INT_PTR CALLBACK proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
		{
			if( msg == WM_CLOSE ) {
				DestroyWindow( hwnd );
				return TRUE;
			}
			else if( msg == WM_DESTROY ) {
				PostQuitMessage( 0 );
				return TRUE;
			}

			return FALSE;
		}
	};

} } // namespace mmaccel::key_config