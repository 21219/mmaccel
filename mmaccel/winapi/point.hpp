#pragma once

#include "../platform.hpp"

namespace winapi
{
	enum struct origin_type
	{
		client,
		screen,
	};

	namespace detail
	{
		template <typename T>
		class point_impl
		{
			T& this_() noexcept
			{
				return *static_cast<T*>( this );
			}

			T const& this_() const noexcept
			{
				return *static_cast<T const*>( this );
			}

		public:
			LONG& x() noexcept
			{
				return this_().data().x;
			}

			LONG const& x() const noexcept
			{
				return this_().data().x;
			}

			LONG& y() noexcept
			{
				return this_().data().y;
			}

			LONG const& y() const noexcept
			{
				return this_().data().y;
			}
		};

	} // namespace detail

	template <origin_type Origin>
	class point_t;

	template <>
	class point_t< origin_type::screen > :
		public detail::point_impl< point_t< origin_type::screen > >
	{
		POINT pt_;

	public:
		point_t() = default;
		point_t( point_t const& ) = default;
		point_t( point_t&& ) = default;
		point_t& operator=( point_t const& ) = default;
		point_t& operator=( point_t&& ) = default;

		point_t( LONG x, LONG y ) :
			pt_{ x, y }
		{}

		point_t(POINT const& pt) :
			pt_{ pt.x, pt.y }
		{ }

		POINT& data() noexcept
		{
			return pt_;
		}

		POINT const& data() const noexcept
		{
			return pt_;
		}
	};
	
	template <>
	class point_t< origin_type::client > :
		public detail::point_impl< point_t< origin_type::client > >
	{
		POINT pt_;
		HWND hwnd_;

	public:
		point_t() = default;
		point_t( point_t const& ) = default;
		point_t( point_t&& ) = default;
		point_t& operator=( point_t const& ) = default;
		point_t& operator=( point_t&& ) = default;

		point_t(HWND hwnd, LONG x, LONG y) :
			pt_{ x, y },
			hwnd_( hwnd )
		{ }

		point_t(HWND hwnd, POINT const& pt) :
			pt_{ pt.x, pt.y },
			hwnd_( hwnd )
		{ }

		POINT& data() noexcept
		{
			return pt_;
		}

		POINT const& data() const noexcept
		{
			return pt_;
		}

		HWND window_handle() const noexcept
		{
			return hwnd_;
		}
	};

	template <origin_type Origin>
	inline bool operator==( point_t<Origin> const& lhs, point_t<Origin> const& rhs ) noexcept
	{
		return lhs.x() == rhs.x() && lhs.y() == rhs.y();
	}

	template <origin_type Origin>
	inline bool operator!=( point_t<Origin> const& lhs, point_t<Origin> const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}

	template <origin_type Origin>
	inline bool operator<( point_t<Origin> const& lhs, point_t<Origin> const& rhs ) noexcept
	{
		return lhs.x() * lhs.x() + lhs.y() * lhs.y() < rhs.x() * rhs.x() + rhs.y() * rhs.y();
	}

	template <origin_type Origin>
	inline bool operator>=( point_t<Origin> const& lhs, point_t<Origin> const& rhs ) noexcept
	{
		return !( lhs < rhs );
	}

	template <origin_type Origin>
	inline bool operator>( point_t<Origin> const& lhs, point_t<Origin> const& rhs ) noexcept
	{
		return rhs < lhs;
	}

	template <origin_type Origin>
	inline bool operator<=( point_t<Origin> const& lhs, point_t<Origin> const& rhs ) noexcept
	{
		return !( lhs > rhs );
	}

} // namespace winap