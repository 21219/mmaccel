#pragma once

#include "../platform.hpp"

namespace winapi
{
	class thread_id
	{
		DWORD id_;

	public:
		thread_id() = default;
		thread_id( thread_id const& ) = default;
		thread_id( thread_id&& ) = default;
		thread_id& operator=( thread_id const& ) = default;
		thread_id& operator=( thread_id&& ) = default;

		thread_id( DWORD id ) noexcept : id_( id )
		{ }

		DWORD get() const noexcept
		{
			return id_;
		}
	};

	inline bool operator==( thread_id const& lhs, thread_id const& rhs ) noexcept
	{
		return lhs.get() == rhs.get();
	}

	inline bool operator!=( thread_id const& lhs, thread_id const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}

	inline bool operator<( thread_id const& lhs, thread_id const& rhs ) noexcept
	{
		return lhs.get() < rhs.get();
	}

	inline bool operator>=( thread_id const& lhs, thread_id const& rhs ) noexcept
	{
		return !( lhs < rhs );
	}

	inline bool operator>( thread_id const& lhs, thread_id const& rhs ) noexcept
	{
		return rhs < lhs;
	}

	inline bool operator<=( thread_id const& lhs, thread_id const& rhs ) noexcept
	{
		return !( lhs > rhs );
	}

	inline thread_id get_current_thread_id() noexcept
	{
		return GetCurrentThreadId();
	}

	inline thread_id get_thread_id( HWND hwnd ) noexcept
	{
		return GetWindowThreadProcessId( hwnd, nullptr );
	}

} // namespace winapi