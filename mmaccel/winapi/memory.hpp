#pragma once

#include "../platform.hpp"
#include <memory>

namespace winapi
{
	struct local_free_deleter
	{
		template <typename Handle>
		void operator()( Handle p ) const noexcept
		{
			LocalFree( p );
		}
	};

} // namespace winapi