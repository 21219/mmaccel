#include "stdafx.h"
#include "CppUnitTest.h"
#include <atomic>
#include <fstream>
#include <mmaccel/file_monitor.hpp>
#include <mmaccel/winapi/directory.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mmaccel
{		
	TEST_CLASS(FileMonitorTest)
	{
	public:
		TEST_METHOD(Execute)
		{
			file_monitor fm;
			std::atomic< bool > flg = false;

			fm.start( winapi::get_module_path(), [&flg]( boost::string_ref ) { 
				flg = true;
				return true;
			} );

			std::ofstream ofs( "test.txt" );
			ofs << "aaa" << std::endl;
			ofs.close();

			fm.stop();
			fm.join();

			Assert::IsTrue( flg );

			DeleteFileW( L"test.txt" );
		}
	};
}