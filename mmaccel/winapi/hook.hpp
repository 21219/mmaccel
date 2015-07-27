#pragma once

#include "../platform.hpp"
#include <memory>

namespace winapi
{
	namespace detail
	{
		struct unhook_deleter
		{
			void operator()( HHOOK h ) const noexcept
			{
				UnhookWindowsHookEx( h );
			}
		};

	} // namespace detail

	using hook_handle = std::unique_ptr< typename std::remove_pointer< HHOOK >::type, detail::unhook_deleter >;

	enum struct hook_type
	{
		call_wnd_proc = WH_CALLWNDPROC,
		get_message = WH_GETMESSAGE,
	};

	inline hook_handle set_windows_hook_ex( hook_type id, HOOKPROC proc, HINSTANCE hinst, DWORD th_id ) noexcept
	{
		return hook_handle( SetWindowsHookExW( static_cast< int >( id ), proc, hinst, th_id ) );
	}

} // namespace winapi