#pragma once

#include "../platform.hpp"
#include <boost/utility/string_ref.hpp>

namespace winapi
{
	inline HMODULE load_library( boost::wstring_ref path ) noexcept
	{
		return LoadLibraryW( path.data() );
	}

	inline HMODULE get_module_handle() noexcept
	{
		return GetModuleHandleW( nullptr );
	}

	inline HMODULE get_module_handle( boost::wstring_ref path ) noexcept
	{
		return GetModuleHandleW( path.data() );
	}

	template <typename F>
	inline F get_proc_address( HMODULE mod, boost::string_ref name ) noexcept
	{
		return reinterpret_cast<F>( GetProcAddress( mod, name.data() ) );
	}

} // namespace winapi