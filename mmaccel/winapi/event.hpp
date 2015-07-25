#pragma once

#include "handle.hpp"
#include <boost/utility/string_ref.hpp>

namespace winapi
{
	template <bool Manual>
	class event_handle
	{
		unique_handle handle_;

	public:
		event_handle() = default;

		event_handle( bool state ) :
			handle_( CreateEvent( nullptr, static_cast< BOOL >( Manual ), static_cast< BOOL >( state ), nullptr ) )
		{}

		bool empty() const noexcept
		{
			return !handle_;
		}

		void set() const noexcept
		{
			SetEvent( handle_.get() );
		}

		void reset() const noexcept
		{
			ResetEvent( handle_.get() );
		}

		HANDLE get() const noexcept
		{
			return handle_.get();
		}
	};

	using manual_reset_event = event_handle< true >;
	using auto_reset_event = event_handle< false >;

} // namespace winapi