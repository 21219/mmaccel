#pragma once

#include "../resource.h"
#include "../winapi/dialog.hpp"
#include "../winapi/directory.hpp"

namespace mmaccel
{
	class version_dialog
	{
	public:
		static void show( HWND parent ) noexcept
		{
			winapi::modal_dialog_box( winapi::get_module_path() + L"\\mmaccel\\mmaccel.dll", IDD_MMACCEL_VERSION, parent, proc );
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