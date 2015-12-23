#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/message_box.hpp>
#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/winapi/string.hpp>
#include <mmaccel/winapi/command_line.hpp>
#include <mmaccel/v120_to_v150.hpp>
#include <clocale>
#include <locale>
#include <sstream>
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

void write_key_config_window( HANDLE hwrite, HWND wnd )
{
	auto const p = reinterpret_cast<std::uintptr_t>( wnd );
	DWORD sz;

	if( !WriteFile( hwrite, &p, sizeof( std::uintptr_t ), &sz, nullptr ) ) {
		winapi::last_error_message_box( u8"MMAccel key_config", u8"WriteFile error" );
		return;
	}
}

void take_over()
{
	auto const path = winapi::get_module_path();

	mmaccel::mmaccel_txt_to_key_map_txt(
		path.substr( 0, path.rfind( u8"\\" ) ) + u8"\\mmaccel.txt",
		winapi::get_module_path() + u8"\\key_map.txt",
		mmaccel::mmd_map::load( winapi::get_module_path() + u8"\\mmd_map.json" )
	);
}

int WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
	try {
		INITCOMMONCONTROLSEX icc_ex = {
			sizeof( INITCOMMONCONTROLSEX ),
			ICC_WIN95_CLASSES
		};
		InitCommonControlsEx( &icc_ex );

		std::locale::global( std::locale( "ja-JP" ) );
		std::setlocale( LC_ALL, "ja-JP" );

		take_over();

		STARTUPINFOW si;
		GetStartupInfoW( &si );

		mmaccel::key_config::window().show( si.dwX, si.dwY );
	
		auto const args = winapi::get_command_line_args();
		if( !args.empty() && args[0] == L"--mmd" ) {
			write_key_config_window( si.hStdOutput, mmaccel::key_config::window().handle() );
		}

		MSG msg;
		for( ;; ) {
			auto const result = GetMessageW( &msg, nullptr, 0, 0 );
			if( result == 0 || result == -1 ) {
				break;
			}
			if( IsDialogMessageW( mmaccel::key_config::window().handle(), &msg ) ) {
				continue;
			}

			DispatchMessageW( &msg );
		}
	}
	catch( std::exception const& e ) {
		STARTUPINFOW si;
		GetStartupInfoW( &si );

		write_key_config_window( si.hStdOutput, nullptr );
		winapi::message_box( u8"MMAccel", e.what(), MB_OK | MB_ICONERROR );
	}

	return 0;
}