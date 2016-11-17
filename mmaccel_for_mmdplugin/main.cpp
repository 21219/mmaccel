#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/module.hpp>
#include <mmaccel/winapi/directory.hpp>
#include <mmaccel/mmaccel.hpp>
#include <mmaccel/path.hpp>
#include "MMDPlugin/mmd_plugin.h"

class mmaccel_for_mmdplugin :
	public MMDPluginDLL1
{
	HMODULE mod_;

public:
	mmaccel_for_mmdplugin() :
		mod_( winapi::load_library( winapi::get_module_path() + "\\plugin\\mmaccel\\mmaccel.dll" ) )
	{
		if( mod_ ) {
			auto const hooks = winapi::get_proc_address< decltype( &mmaccel_register_hooks ) >( mod_, "mmaccel_register_hooks" );
			auto const start = winapi::get_proc_address< decltype( &mmaccel_start ) >( mod_, "mmaccel_start" );

			if( hooks && start ) {
				auto const path = winapi::get_module_path() + "\\plugin\\mmaccel";
				hooks( path.c_str() );
				start( path.c_str() );
			}
		}
	}
};

extern "C" 
{
	MMD_PLUGIN_API int version()
	{
		return 1;
	}

	MMD_PLUGIN_API MMDPluginDLL1* create1(IDirect3DDevice9*)
	{
		return new mmaccel_for_mmdplugin;
	}

	MMD_PLUGIN_API void destroy1(MMDPluginDLL1* p)
	{
		delete p;
	}
}