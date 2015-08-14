#pragma once

#include "../platform.hpp"
#include "string.hpp"
#include "process.hpp"
#include <vector>

namespace winapi
{
	namespace detail
	{
		inline BOOL CALLBACK enum_windows_callback( HWND hwnd, LPARAM lparam )
		{
			auto& wnds = *reinterpret_cast<std::vector< HWND >*>( lparam );
			wnds.push_back( hwnd );
			return TRUE;
		}

	} // namespace detail

	inline std::vector< HWND > enum_windows()
	{
		std::vector< HWND > wnds;

		if( !EnumWindows( detail::enum_windows_callback, reinterpret_cast<LPARAM>( &wnds ) ) ) {
			return{};
		}

		return wnds;
	}

	inline std::vector< HWND > enum_child_windows(HWND parent)
	{
		std::vector< HWND > wnds;

		if( !EnumChildWindows( parent, detail::enum_windows_callback, reinterpret_cast<LPARAM>( &wnds ) ) ) {
			return{};
		}

		return wnds;
	}

	inline HWND get_window_from_process_id( process_id const& id )
	{
		auto const wnds = enum_windows();

		for( HWND w : wnds ) {
			if( id == get_process_id( w ) ) {
				return w;
			}
		}

		return nullptr;
	}

	inline std::string get_class_name( HWND hwnd )
	{
		wchar_t tmp[256];

		if( !GetClassNameW( hwnd, tmp, std::extent< decltype( tmp ) >::value ) ) {
			return{};
		}

		return winapi::widechar_to_multibyte( tmp, CP_UTF8 );
	}

	inline RECT get_client_rect( HWND hwnd )
	{
		RECT rc;
		GetClientRect( hwnd, &rc );

		return rc;
	}

} // namespace winapi