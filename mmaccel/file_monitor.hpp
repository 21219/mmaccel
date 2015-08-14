#pragma once

#include "platform.hpp"
#include <memory>
#include <functional>
#include <boost/utility/string_ref.hpp>
#include "winapi/event.hpp"

namespace mmaccel
{
	class file_monitor
	{
		class impl;
		std::shared_ptr< impl > p_;

	public:
		file_monitor();
		void start(boost::string_ref dir, std::function< bool (boost::string_ref) >&&);
		void stop() noexcept;
		void join();
	};

} // namespace mmaccel