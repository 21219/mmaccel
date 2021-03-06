#pragma once

#include "../winapi/menu.hpp"
#include "../winapi/directory.hpp"
#include "../utility/type_tuple.hpp"
#include "message_handler.hpp"
#include "../resource.h"

namespace mmaccel
{
	template <DWORD ControlID>
	struct menu_command
	{
		using function_type = void();
		static constexpr auto id = ControlID;
	};

	template <DWORD ID, DWORD... SubItems>
	class menu_t
	{
		winapi::loaded_menu_handle root_;
		message_handler< menu_command< SubItems >... > mh_;

	public:
		menu_t() = default;

		menu_t( HWND hwnd, boost::string_ref path, boost::string_ref root_name ) :
			root_( winapi::load_menu( path, ID ) )
		{
			auto const src = winapi::get_menu( hwnd );
			if( !src ) {
				winapi::message_box( u8"MMAccel", u8"メニューハンドルを取得できません", MB_OK | MB_ICONERROR );
			}

			int index = 0;
			for(  ; index < winapi::get_menu_item_count( src ); ++index ) {
				MENUITEMINFO info;
				info.cbSize = sizeof( info );
				info.fMask = MIIM_FTYPE;
				GetMenuItemInfo( src.get(), index, TRUE, &info );
				if( info.fType & MFT_RIGHTJUSTIFY ) {
					break;
				}
			}

			if( src && !winapi::insert_menu( src, index, winapi::get_sub_menu( root_, 0 ), root_name ) ) {
				winapi::last_error_message_box( u8"MMAccel", u8"insert_menu error" );
			}

			DrawMenuBar( hwnd );
		}

		template <typename Message, typename F>
		void assign_handler(Message, F&& f)
		{
			mh_.assign( Message(), std::forward< F >( f ) );
		}

		void on_command( WPARAM wparam )
		{
			if( !root_ ) {
				return;
			}

			if( HIWORD( wparam ) != 0 ) {
				return;
			}

			invoke( mh_, LOWORD( wparam ) );
		}

	private:
		template <typename... Messages>
		static void invoke( message_handler< Messages... > const& mh, DWORD id )
		{
			invoke_impl( type_tuple< Messages... >(), mh, id );
		}

		template <typename Message, typename... Rest, typename... MH>
		static void invoke_impl( type_tuple< Message, Rest... >, message_handler<MH...> const& mh, DWORD id )
		{
			if( id == Message::id ) {
				mh.invoke( Message() );
				return;
			}

			invoke_impl( type_tuple< Rest... >(), mh, id );
		}

		template <typename... MH>
		static void invoke_impl( type_tuple<>, message_handler< MH... > const&, DWORD )
		{ }
	};

} // namespace mmaccel
