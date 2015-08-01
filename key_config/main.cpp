#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/message_box.hpp>
#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/winapi/string.hpp>
#include <clocale>
#include <locale>
#include "resource.h"

INT_PTR CALLBACK procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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

int WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int showcmd )
{
	try {
		HWND const hwnd = winapi::modeless_dialog_box( IDD_KEY_CONFIG, nullptr, &procedure );
		if( !hwnd ) {
			throw std::runtime_error( "cannot create dialog" );
		}

		ShowWindow( hwnd, showcmd );

		MSG msg;
		for( ;; ) {
			auto const result = GetMessageW( &msg, nullptr, 0, 0 );
			if( result == 0 || result == -1 ) {
				break;
			}

			DispatchMessageW( &msg );
		}
	}
	catch( std::exception const& e ) {
		winapi::message_box( L"MMAccel", winapi::multibyte_to_widechar( e.what(), CP_OEMCP ), MB_OK | MB_ICONERROR );
	}

	return 0;
}