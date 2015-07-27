#pragma once

#include "../platform.hpp"

namespace winapi
{
	class process_id
	{
		DWORD id_;

	public:
		process_id() = default;
		process_id( process_id const& ) = default;
		process_id( process_id&& ) = default;
		process_id& operator=( process_id const& ) = default;
		process_id& operator=( process_id&& ) = default;

		process_id( DWORD id ) noexcept : id_( id )
		{ }

		DWORD get() const noexcept
		{
			return id_;
		}
	};

	inline bool operator==( process_id const& lhs, process_id const& rhs ) noexcept
	{
		return lhs.get() == rhs.get();
	}

	inline bool operator!=( process_id const& lhs, process_id const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}
	
	inline bool operator<( process_id const& lhs, process_id const& rhs ) noexcept
	{
		return lhs.get() < rhs.get();
	}

	inline bool operator>=( process_id const& lhs, process_id const& rhs ) noexcept
	{
		return !( lhs < rhs );
	}

	inline bool operator>( process_id const& lhs, process_id const& rhs ) noexcept
	{
		return rhs < lhs;
	}

	inline bool operator<=( process_id const& lhs, process_id const& rhs ) noexcept
	{
		return !( lhs > rhs );
	}

	inline process_id get_current_process_id() noexcept
	{
		return GetCurrentProcessId();
	}

	inline process_id get_process_id( HWND hwnd ) noexcept
	{
		DWORD id;
		GetWindowThreadProcessId( hwnd, &id );
		return id;
	}
	
} // namespace winapi