#include "platform.hpp"

namespace mmaccel
{
	void start()
	{
		OutputDebugStringW( L"mmaccel::start\n" );
	}

} // namespace mmaccel

extern "C"
{
	__declspec( dllexport ) void start_mmaccel()
	{
		mmaccel::start();
	}

} // extern "C"