#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/message_box.hpp>
#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/winapi/string.hpp>
#include <clocale>
#include <locale>
#include "window.hpp"
#include "resource.h"

#ifdef UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

int WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
	try {
		INITCOMMONCONTROLSEX icc_ex = {
			sizeof( INITCOMMONCONTROLSEX ),
			ICC_WIN95_CLASSES
		};
		InitCommonControlsEx( &icc_ex );
	
		mmaccel::key_config::window().show();

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
		winapi::message_box( L"MMAccel", winapi::multibyte_to_widechar( e.what(), CP_UTF8 ), MB_OK | MB_ICONERROR );
	}

	return 0;
}