#pragma once

#include "platform.hpp"
#include "winapi/menu.hpp"
#include "winapi/control.hpp"
#include "keyboard.hpp"
#include "mmd_map.hpp"

namespace mmaccel
{
	inline void button( HWND ctrl )
	{
		if( IsWindowVisible( ctrl ) && IsWindowEnabled( ctrl ) ) {
			SendMessageW( ctrl, BM_CLICK, 0, 0 );
		}
	}

	inline void fold( HWND hide_ctrl, HWND show_ctrl )
	{
		if( IsWindowVisible( hide_ctrl ) ) {
			SendMessageW( hide_ctrl, BM_CLICK, 0, 0 );
		}
		else {
			SendMessageW( show_ctrl, BM_CLICK, 0, 0 );
		}
	}

	inline void edit( HWND ctrl )
	{
		if( IsWindowVisible( ctrl ) && IsWindowEnabled( ctrl ) ) {
			SetFocus( ctrl );
		}
	}

	inline void combo_prev( HWND ctrl )
	{
		if( !IsWindowVisible( ctrl ) || !IsWindowEnabled( ctrl ) ) {
			return;
		}

		auto const index = SendMessageW( ctrl, CB_GETCURSEL, 0, 0 );
		if( index - 1 >= 0 ) {
			SendMessageW( ctrl, CB_SETCURSEL, index - 1, 0 );
			SendMessageW( GetParent( ctrl ), WM_COMMAND, MAKEWPARAM( winapi::get_dlg_ctrl_id( ctrl )->get(), CBN_SELCHANGE ), reinterpret_cast<LPARAM>( ctrl ) );
		}
	}

	inline void combo_next( HWND ctrl )
	{
		if( !IsWindowVisible( ctrl ) || !IsWindowEnabled( ctrl ) ) {
			return;
		}
		
		auto const index = SendMessageW( ctrl, CB_GETCURSEL, 0, 0 );
		auto const sz = SendMessageW( ctrl, CB_GETCOUNT, 0, 0 );
		if( index + 1 < sz ) {
			SendMessageW( ctrl, CB_SETCURSEL, index + 1, 0 );
			SendMessageW( GetParent( ctrl ), WM_COMMAND, MAKEWPARAM( winapi::get_dlg_ctrl_id( ctrl )->get(), CBN_SELCHANGE ), reinterpret_cast<LPARAM>( ctrl ) );
		}
	}

	inline void press_key( keys_combination const& kc, keyboard_state& ks )
	{
		for( auto const i : kc ) {
			ks[i] = true;
		}
	}

	inline void menu( HWND mmd, int index, int sub_index )
	{
		auto const m = winapi::get_sub_menu( winapi::get_menu( mmd ), index );
		if( !m ) {
			return;
		}

		auto const state = winapi::get_menu_state( m, sub_index );
		if( !state ) {
			return;
		}

		if( !( *state & MFS_DISABLED ) ) {
			SendMessageW( mmd, WM_COMMAND, MAKEWPARAM( GetMenuItemID( m.get(), sub_index ), 0 ), 0 );
		}
	}

	inline void kill_focus( HWND mmd )
	{
		SetFocus( mmd );
	}

	inline void nothing()
	{ }

	struct handler_t
	{
		std::string name;
		std::function< void( keyboard_state& ) > func;

		handler_t() = default;

		template <typename F>
		handler_t( boost::string_ref n, F&& f ) :
			name( n ), 
			func( std::forward< F >( f ) )
		{ }
	};

	inline handler_t make_handler(json::pair_type const& elem, HWND mmd)
	{
		auto const f_type = boost::get< std::string >( boost::get< json::array_type >( elem.second )[1] );

		auto ctrl_handle = [mmd]( json::data_type const& val, int index ) {
			return winapi::get_dlg_item( mmd, std::strtol( boost::get< std::string >( boost::get< json::array_type >( val )[index] ).c_str(), nullptr, 16 ) );
		};

		if( f_type == u8"button" ) {
			auto const ctrl = ctrl_handle( elem.second, 2 );

			return{ f_type, [ctrl]( keyboard_state& ) { button( ctrl ); } };
		}
		else if( f_type == u8"fold" ) {
			auto const hide_ctrl = ctrl_handle( elem.second, 2 );
			auto const show_ctrl = ctrl_handle( elem.second, 3 );

			return{ f_type, [hide_ctrl, show_ctrl]( keyboard_state& ) { fold( hide_ctrl, show_ctrl ); } };
		}
		else if( f_type == u8"edit" ) {
			auto const ctrl = ctrl_handle( elem.second, 2 );

			return{ f_type, [ctrl]( keyboard_state& ) { edit( ctrl ); } };
		}
		else if( f_type == u8"combo_prev" ) {
			auto const ctrl = ctrl_handle( elem.second, 2 );
			
			return{ f_type, [ctrl]( keyboard_state& ) { combo_prev( ctrl ); } };
		}
		else if( f_type == u8"combo_next" ) {
			auto const ctrl = ctrl_handle( elem.second, 2 );
			
			return{ f_type, [ctrl]( keyboard_state& ) { combo_next( ctrl ); } };
		}
		else if( f_type == u8"key" ) {
			keys_combination kc;
			auto const a = boost::get< json::array_type >( elem.second );
			for( std::size_t i = 2; i < a.size(); ++i ) {
				kc.push_back( std::strtol( boost::get< std::string >( a[i] ).c_str(), nullptr, 16 ) );
			}

			return{ f_type, [kc]( keyboard_state& ks ) { press_key( kc, ks );  } };
		}
		else if( f_type == u8"menu" ) {
			int const index = boost::get< int >( boost::get< json::array_type >( elem.second )[2] );
			int const sub_index = boost::get< int >( boost::get< json::array_type >( elem.second )[3] );

			return{ f_type, [mmd, index, sub_index]( keyboard_state& ) { menu( mmd, index, sub_index ); } };
		}
		else if( f_type == u8"kill_focus" ) {
			return{ f_type, [mmd]( keyboard_state& ) { kill_focus( mmd ); } };
		}
		else if( f_type == u8"nothing" ) {
			return{ f_type, []( keyboard_state& ) {} };
		}

		return{};
	}

} // namespace mmaccel