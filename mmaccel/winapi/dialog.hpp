#pragma once

#include "module.hpp"
#include "control.hpp"
#include <boost/utility/string_ref.hpp>

namespace winapi
{
	inline void modal_dialog_box( boost::string_ref path, control_id const& id, HWND parent, DLGPROC proc ) noexcept
	{
		DialogBoxW( get_module_handle( path ), MAKEINTRESOURCEW( id.get() ), parent, proc );
	}

	inline HWND modeless_dialog_box( boost::string_ref path, control_id const& id, HWND parent, DLGPROC proc ) noexcept
	{
		return CreateDialogW( get_module_handle( path ), MAKEINTRESOURCEW( id.get() ), parent, proc );
	}

	inline HWND modeless_dialog_box( control_id const& id, HWND parent, DLGPROC proc ) noexcept
	{
		return CreateDialogW( get_module_handle(), MAKEINTRESOURCEW( id.get() ), parent, proc );
	}

} // namespace winapi