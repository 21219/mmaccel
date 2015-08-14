#pragma once

#include "../platform.hpp"
#include "string.hpp"
#include <boost/utility/string_ref.hpp>

namespace winapi
{
	inline HMODULE load_library( boost::string_ref path ) 
	{
		return LoadLibraryW( winapi::multibyte_to_widechar( path, CP_UTF8 ).c_str() );
	}

	inline HMODULE get_module_handle() noexcept
	{
		return GetModuleHandleW( nullptr );
	}

	inline HMODULE get_module_handle( boost::string_ref path ) noexcept
	{
		return GetModuleHandleW( winapi::multibyte_to_widechar( path, CP_UTF8 ).c_str() );
	}

	template <typename F>
	inline F get_proc_address( HMODULE mod, boost::string_ref name ) noexcept
	{
		return reinterpret_cast<F>( GetProcAddress( mod, name.data() ) );
	}

} // namespace winapi