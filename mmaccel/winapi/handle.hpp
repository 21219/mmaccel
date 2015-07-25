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

} // namespace winapi