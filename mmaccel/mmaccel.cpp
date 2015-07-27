#include "platform.hpp"
#include "winapi/window.hpp"
#include "gui/menu.hpp"
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
			}
			catch( ... ) {
			}

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}
	};

	class module_impl
	{
		HWND mmd_;
		menu m_;

		module_impl()
		{}

		void call_window_proc( CWPSTRUCT const& cwp )
		{

		}

		void get_message_proc(MSG& msg)
		{

		}

	public:	
		void start()
		{
			mmd_ = winapi::get_window_from_process_id( winapi::get_current_process_id() );
			m_ = menu( mmd_ );
		}

		static module_impl& instance()
		{
			static std::unique_ptr< module_impl > obj( new module_impl );
			return *obj;
		}

	private:
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
	};

	inline module_impl& module()
	{
		return module_impl::instance();
	}

} // namespace mmaccel

extern "C"
{
	__declspec( dllexport ) void start_mmaccel()
	{
		mmaccel::module().start();
	}

} // extern "C"