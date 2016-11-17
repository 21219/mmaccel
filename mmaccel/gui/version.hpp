#pragma once

#include "../resource.h"
#include "../winapi/dialog.hpp"
#include "../winapi/directory.hpp"
#include "../path.hpp"

namespace mmaccel
{
	class version_dialog
	{
	public:
		static void show( std::string const& path, HWND parent ) noexcept
		{
			winapi::modal_dialog_box( path, IDD_MMACCEL_VERSION, parent, proc );
		}

	private:
		static INT_PTR CALLBACK proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM )
		{
			if( msg == WM_DESTROY || ( msg == WM_COMMAND && LOWORD( wparam ) == IDOK ) ) {
				EndDialog( hwnd, IDOK );
				return TRUE;
			}

			return FALSE;
		}
	};

} // namespace mmaccel