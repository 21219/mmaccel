#pragma once

#include "../platform.hpp"
#include <memory>

namespace winapi
{
	namespace detail
	{
		struct close_handle_deleter
		{
			void operator()( HANDLE h ) const noexcept
			{
				CloseHandle( h );
			}
		};

	} // namespace detail

	using unique_handle = std::unique_ptr< typename std::remove_pointer< HANDLE >::type, detail::close_handle_deleter >;

	inline unique_handle duplicate_handle(HANDLE src_process, unique_handle const& src, HANDLE target_process, DWORD access, BOOL inherit, DWORD options)
	{
		HANDLE target;

		if( !DuplicateHandle( src_process, src.get(), target_process, &target, access, inherit, options ) ) {
			return{};
		}

		return{ target, detail::close_handle_deleter() };
	}

} // namespace winapi