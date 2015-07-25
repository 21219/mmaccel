#pragma once

#include "../platform.hpp"
#include "event.hpp"
#include "directory.hpp"
#include <boost/optional.hpp>

namespace winapi
{
	struct overlapped_t
	{
		OVERLAPPED obj;

		overlapped_t() = default;

		overlapped_t( manual_reset_event const& e ) noexcept
		{
			ZeroMemory( &obj, sizeof( OVERLAPPED ) );
			obj.hEvent = e.get();
		}
	};

	inline boost::optional< DWORD > get_overlapped_result(directory_handle const& dir, overlapped_t& ol, bool wait_flag) noexcept
	{
		DWORD sz = 0;

		if( !GetOverlappedResult( dir.get(), &ol.obj, &sz, wait_flag ) ) {
			return{};
		}

		return sz;
	}

} // namespace winapi