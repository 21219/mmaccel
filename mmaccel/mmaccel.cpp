#include "platform.hpp"
#include "winapi/window.hpp"
#include "winapi/hook.hpp"
#include "winapi/thread.hpp"
#include "winapi/message_box.hpp"
#include "winapi/string.hpp"
#include "path.hpp"
#include "file_monitor.hpp"
#include "resource.h"
#include "key_map.hpp"
#include "gui/menu.hpp"
#include "gui/version.hpp"
#include <atomic>
#include <sstream>
#include <memory>

namespace mmaccel
{
	template <HOOKPROC Func>
	class hook_procedure
	{
	public:
		inline static HOOKPROC address() noexcept
		{
			return &impl;
		}

	private:
		static LRESULT CALLBACK impl( int code, WPARAM wparam, LPARAM lparam ) noexcept
		{
			if( code < 0 ) {
				return CallNextHookEx( nullptr, code, wparam, lparam );
			}

			try {
				return Func( code, wparam, lparam );
			}
			catch( std::exception const& e ) {
				winapi::message_box( u8"MMAccel", e.what(), MB_OK | MB_ICONERROR );
			}
			catch( ... ) {
				winapi::message_box( u8"MMaccel", u8"unknown error", MB_OK | MB_ICONERROR );
			}

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}
	};

	class module_impl
	{
		HWND mmd_;
		HWND sep_;
		menu_t< IDR_MMACCEL_MENU, ID_MMACCEL_SETTING, ID_MMACCEL_ERROR_LOG, ID_MMACCEL_VERSION > menu_;

		bool dialog_;
		std::atomic< bool > update_;

		file_monitor fm_;
		key_handler_map_t khm_;

		winapi::hook_handle hook_cwp_;
		winapi::hook_handle hook_gm_;

		module_impl():
			dialog_( false ),
			update_( false )
		{}

	public:	
		void register_hooks()
		{
			HINSTANCE hinst = winapi::get_module_handle();
			hook_cwp_ = winapi::set_windows_hook_ex( 
				winapi::hook_type::call_wnd_proc, hook_procedure< call_wnd_proc_forward>::address(), hinst, winapi::get_current_thread_id() 
			);
			hook_gm_ = winapi::set_windows_hook_ex(
				winapi::hook_type::get_message, hook_procedure< get_msg_proc_forward >::address(), hinst, winapi::get_current_thread_id()
			);
		}

		void start()
		{
			try {
				mmd_ = winapi::get_window_from_process_id( winapi::get_current_process_id() );

				menu_ = decltype( menu_ )( mmd_, dll_path(), u8"MMAccel" );
				menu_.assign_handler( menu_command< ID_MMACCEL_SETTING >(), [this] { this->run_key_config(); } );
				menu_.assign_handler( menu_command< ID_MMACCEL_VERSION >(), [this] { version_dialog::show( this->mmd_ ); } );

				fm_.start( winapi::get_module_path(), [this]( boost::string_ref name ) {
					if( name == u8"mmaccel\\key_map.txt" ) {
						update_ = false;
						PostMessageW( mmd_, WM_NULL, 0, 0 );

						return true;
					}

					return false;
				} );
				
				PostMessageW( mmd_, WM_NULL, 0, 0 );
			}
			catch( std::exception const& e ) {
				winapi::message_box( u8"MMAccel", e.what(), MB_OK | MB_ICONERROR );
			}
		}

		static module_impl& instance()
		{
			static std::unique_ptr< module_impl > obj( new module_impl );
			return *obj;
		}

	private:
		void call_window_proc( CWPSTRUCT const& cwp )
		{
			try {
				if( cwp.message == WM_ACTIVATE && !( LOWORD( cwp.wParam ) == WA_INACTIVE ) ) {
					if( winapi::get_class_name( cwp.hwnd ) == u8"MicWindow" ) {
						sep_ = cwp.hwnd;
					}
					else if( winapi::get_class_name( cwp.hwnd ) == u8"#32770" ) {
						dialog_ = true;
					}
				}
				else if( cwp.message == WM_DESTROY ) {
					if( winapi::get_class_name( cwp.hwnd ) == u8"MicWindow" ) {
						sep_ = nullptr;
					}
					else if( winapi::get_class_name( cwp.hwnd ) == u8"#32770" ) {
						dialog_ = false;
					}
				}
			}
			catch( std::exception const& e ) {
				winapi::message_box( u8"MMAccel", e.what(), MB_OK | MB_ICONERROR );
			}
		}

		void get_message_proc( MSG& msg )
		{
			if( msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN ) {
				if( dialog_ ) {
					return;
				}
				
				auto ks = get_keyboard_state();
				auto const kc = state_to_combination( ks );
				auto const rng = khm_.equal_range( kc );

				if( rng.first == khm_.end() && rng.second == khm_.end() ) {
					return;
				}

				for( auto const i : kc ) {
					ks[i] = false;
				}
				for( auto itr = rng.first; itr != rng.second; itr = std::next( itr ) ) {
					itr->second( ks );
				}

				set_keyboard_state( ks );
				msg.wParam = 0;
			}
			else if( msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP ) {
				if( dialog_ ) {
					return;
				}

				auto keys = get_keyboard_state();
				keys.clear();
				set_keyboard_state( keys );
			}
			else if( msg.message == WM_COMMAND ) {
				menu_.on_command( msg.wParam );
			}
			else if( msg.message == WM_NULL ) {
				update_key_map();
			}
		}

		static LRESULT CALLBACK call_wnd_proc_forward(int code, WPARAM wparam, LPARAM lparam)
		{
			auto& cwp = *reinterpret_cast<CWPSTRUCT*>( lparam );
			instance().call_window_proc( cwp );

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}

		static LRESULT CALLBACK get_msg_proc_forward( int code, WPARAM wparam, LPARAM lparam )
		{
			if( code == HC_NOREMOVE ) {
				return CallNextHookEx( nullptr, code, wparam, lparam );
			}

			auto& msg = *reinterpret_cast<MSG*>( lparam );
			instance().get_message_proc( msg );

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}

		void update_key_map()
		{
			if( update_ ) {
				return;
			}
			
			auto const mm = mmd_map::load( u8"mmaccel\\mmd_map.json" );
			auto const key_map_path = winapi::get_module_path() + u8"\\mmaccel\\key_map.txt";
			if( !PathFileExistsW( winapi::multibyte_to_widechar( key_map_path, CP_UTF8 ).c_str() ) ) {
				save_key_map( key_map_path, {}, mm );
			}
		
			khm_ = load_key_handler_map( u8"mmaccel\\key_map.txt", mm, mmd_ );

			update_ = true;
		}

		void run_key_config()
		{
			PROCESS_INFORMATION pi;
			STARTUPINFOW si;

			ZeroMemory( &si, sizeof( si ) );
			si.cb = sizeof( STARTUPINFOW );

			auto const current_dir = winapi::multibyte_to_widechar( winapi::get_module_path() + u8"\\mmaccel", CP_UTF8 );
			if( !CreateProcessW( L"mmaccel\\key_config.exe", nullptr, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, current_dir.c_str(), &si, &pi ) ) {
				return;
			}

			CloseHandle( pi.hThread );
			CloseHandle( pi.hProcess );
		}
	};

	inline module_impl& module()
	{
		return module_impl::instance();
	}

} // namespace mmaccel

extern "C"
{
	__declspec( dllexport ) void mmaccel_register_hooks()
	{
		mmaccel::module().register_hooks();
	}

	__declspec( dllexport ) void mmaccel_start()
	{
		mmaccel::module().start();
	}

} // extern "C"