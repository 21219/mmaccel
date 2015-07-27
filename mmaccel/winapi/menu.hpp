#pragma once

#include "../platform.hpp"
#include "module.hpp"
#include <memory>
#include <type_traits>
#include <boost/utility/string_ref.hpp>

namespace winapi
{
	namespace detail
	{
		struct destroy_menu_deleter
		{
			void operator()( HMENU p ) const noexcept
			{
				DestroyMenu( p );
			}
		};

		struct get_menu_deleter
		{
			void operator()( HMENU ) const noexcept
			{ }
		};

	} // namespace detail

	template <typename Deleter>
	using menu_handle = std::unique_ptr< typename std::remove_pointer< HMENU >::type, Deleter >;

	using loaded_menu_handle = menu_handle< detail::destroy_menu_deleter >;
	using got_menu_handle = menu_handle< detail::get_menu_deleter >;

	inline loaded_menu_handle load_menu( boost::wstring_ref path, UINT id ) noexcept
	{
		return loaded_menu_handle( LoadMenuW( get_module_handle( path ), MAKEINTRESOURCEW( id ) ) );
	}

	inline got_menu_handle get_menu(HWND hwnd) noexcept
	{
		return got_menu_handle( GetMenu( hwnd ) );
	}

	template <typename Deleter>
	inline got_menu_handle get_sub_menu( menu_handle<Deleter> const& m, int index ) noexcept
	{
		return got_menu_handle( GetSubMenu( m.get(), index ) );
	}

	template <typename D0, typename D1>
	inline void insert_menu( menu_handle< D0 > const& m, UINT index, menu_handle< D1 > const& insert_m, boost::wstring_ref str ) noexcept
	{
		std::wstring tmp( str.begin(), str.end() );

		MENUITEMINFOW info;
		ZeroMemory( &info, sizeof( info ) );

		info.cbSize = sizeof( info );
		info.fMask = MIIM_STRING | MIIM_SUBMENU;
		info.dwTypeData = &tmp[0];
		info.hSubMenu = insert_m.get();

		InsertMenuItemW( m.get(), index, TRUE, &info );
	}

	template <typename Deleter>
	inline int get_menu_item_count( menu_handle< Deleter > const& m ) noexcept
	{
		return GetMenuItemCount( m.get() );
	}
 
} // namespace winapi