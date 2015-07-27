#pragma once

#include "../platform.hpp"
#include <boost/optional.hpp>

namespace winapi
{
	class control_id
	{
		int id_;

	public:
		control_id() = default;
		control_id( control_id const& ) = default;
		control_id( control_id&& ) = default;
		control_id& operator=( control_id const& ) = default;
		control_id& operator=( control_id&& ) = default;

		control_id(int id) noexcept : id_( id )
		{ }

		int get() const noexcept
		{
			return id_;
		}
	};

	inline bool operator==( control_id const& lhs, control_id const& rhs ) noexcept
	{
		return lhs.get() == rhs.get();
	}

	inline bool operator!=( control_id const& lhs, control_id const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}

	inline bool operator<( control_id const& lhs, control_id const& rhs ) noexcept
	{
		return lhs.get() < rhs.get();
	}

	inline bool operator>=( control_id const& lhs, control_id const& rhs ) noexcept
	{
		return !( lhs < rhs );
	}

	inline bool operator>( control_id const& lhs, control_id const& rhs ) noexcept
	{
		return rhs < lhs;
	}

	inline bool operator<=( control_id const& lhs, control_id const& rhs ) noexcept
	{
		return !( lhs > rhs );
	}

	HWND get_dlg_item( HWND parent, control_id const& id ) noexcept
	{
		return GetDlgItem( parent, id.get() );
	}

	boost::optional< control_id > get_dlg_ctrl_id( HWND hwnd ) noexcept
	{
		int const id = GetDlgCtrlID( hwnd );

		if( id == 0 ) {
			return{};
		}

		return control_id( id );
	}

} // namespace winapi