#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/message_box.hpp>

int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	winapi::message_box( L"MMAccel", L"run key_config.exe", MB_OK );
	return 0;
}