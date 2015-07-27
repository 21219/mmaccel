#pragma once

#include "../winapi/menu.hpp"
#include "../winapi/directory.hpp"
#include "message_handler.hpp"
#include "../utility/type_tuple.hpp"

namespace mmaccel
{
	template <DWORD ControlID>
	struct menu_command
	{
		using function_type = void();
		static constexpr auto id = ControlID;
	};

	class menu
	{
		winapi::loaded_menu_handle root_;
		message_handler<
			menu_command< ID_MMACCEL_SETTING >,
			menu_command< ID_MMACCEL_ERROR_LOG >,
			menu_command< ID_MMACCEL_VERSION >
		> mh_;

	public:
		menu() = default;

		menu( HWND hwnd ) :
			root_( winapi::load_menu( winapi::get_module_path() + L"\\mmaccel\\mmaccel.dll", IDR_MMACCEL_MENU ) )
		{
			auto const mmd_menu = winapi::get_menu( hwnd );
			winapi::insert_menu( mmd_menu, winapi::get_menu_item_count( mmd_menu ), winapi::get_sub_menu( root_, 0 ), L"MMAccel" );

			DrawMenuBar( hwnd );
		}

		void on_command( WPARAM wparam )
		{
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
