#pragma once

#include "../platform.hpp"
#include "handle.hpp"
#include <boost/optional.hpp>

namespace winapi
{
	struct pipe_handles
	{
		unique_handle read_handle;
		unique_handle write_handle;

		pipe_handles(HANDLE hread, HANDLE hwrite) :
			read_handle( hread ),
			write_handle( hwrite )
		{ }
	};

	inline boost::optional< pipe_handles > create_pipe(SECURITY_ATTRIBUTES& sa, DWORD sz = 0)
	{
		HANDLE hread, hwrite;

		if( !CreatePipe( &hread, &hwrite, &sa, sz ) ) {
			return{};
		}

		return pipe_handles( hread, hwrite );
	}

} // namespace winapi