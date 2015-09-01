#pragma once

#include "../platform.hpp"
#include "module.hpp"

namespace winapi
{
	inline HICON load_icon(DWORD id, int cx, int cy)
	{
		return reinterpret_cast<HICON>( LoadImageW( get_module_handle(), MAKEINTRESOURCEW( id ), IMAGE_ICON, cx, cy, LR_SHARED ) );
	}

	inline HICON load_icon( DWORD id )
	{
		return load_icon( id, GetSystemMetrics( SM_CXICON ), GetSystemMetrics( SM_CYICON ) );
	}

	inline HICON load_small_icon( DWORD id )
	{
		return load_icon( id, GetSystemMetrics( SM_CXSMICON ), GetSystemMetrics( SM_CYSMICON ) );
	}

	inline void set_icon(HWND wnd, DWORD id)
	{
		SendMessageW( wnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>( load_icon( id ) ) );
	}

	inline void set_small_icon( HWND wnd, DWORD id )
	{
		SendMessageW( wnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>( load_small_icon( id ) ) );
	}

} // namespace winapi