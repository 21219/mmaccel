#include "file_monitor.hpp"
#include "winapi/file_notify.hpp"
#include <atomic>
#include <thread>

namespace mmaccel
{
	class file_monitor::impl
	{
		std::function< bool( boost::wstring_ref ) > f_;

		std::thread th_;
		std::atomic< bool > exit_flag_;

		winapi::directory_handle dir_;
		winapi::manual_reset_event event_;

	public:
		impl() :
			event_( true )
		{ }
		
		~impl()
		{
			stop();
			if( th_.joinable() ) {
				th_.join();
			}
		}

		void start(boost::wstring_ref dir, std::function< bool( boost::wstring_ref) >&& f)
		{
			if( th_.joinable() ) {
				return;
			}

			f_ = std::move( f );
			dir_ = winapi::directory_handle( dir, FILE_SHARE_READ, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED );
			exit_flag_ = false;

			th_ = std::thread( [this] { this->thread_main(); } );
		}

		void stop() noexcept
		{
			if( !th_.joinable() ) {
				return;
			}

			exit_flag_ = true;
			event_.set();
		}

		void join()
		{
			if( th_.joinable() ) {
				th_.join();
			}
		}

	private:
		void thread_main()
		{
			winapi::file_notify_information_buffer buf( 2048 );

			for( ;; ) {
				winapi::overlapped_t ol( event_ );

				event_.reset();
				if( exit_flag_ ) {
					break;
				}

				if( !winapi::read_directory_change( dir_, buf, true, FILE_NOTIFY_CHANGE_LAST_WRITE, ol ) ) {
					break;
				}
				if( !winapi::get_overlapped_result( dir_, ol, true ) ) {
					break;
				}

				winapi::enum_file_notify_information( buf, f_ );
			}
		}
	};

	file_monitor::file_monitor() :
		p_( new impl )
	{}

	void file_monitor::start( boost::wstring_ref dir, std::function< bool( boost::wstring_ref ) >&& f )
	{
		p_->start( dir, std::move( f ) );
	}

	void file_monitor::stop() noexcept
	{
		p_->stop();
	}

	void file_monitor::join()
	{
		p_->join();
	}

} // namespace mmaccel