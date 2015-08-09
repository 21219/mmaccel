#pragma once

#include "../platform.hpp"
#include "../winapi/string.hpp"
#include <string>


namespace mmaccel
{
	enum struct encoding_t
	{
		oemcp = CP_OEMCP,
		utf8 = CP_UTF8
	};

	template <encoding_t Encoding>
	class string
	{
		template <typename>
		friend class string;

		std::string str_;

	public:
		using size_type = typename std::string::size_type;
		using difference_type = typename std::string::difference_type;
		using iterator = typename std::string::iterator;
		using const_iterator = typename std::string::const_iterator;

		static constexpr auto npos = std::string::npos;

		string() = default;

		string(char const* src) :
			str_( src )
		{ }

		string(wchar_t const* src):
			str_( winapi::widechar_to_multibyte( src, static_cast< int >( Encoding ) ) )
		{ }

		string(std::string const& src) :
			str_( src.begin(), src.end() )
		{ }

		string(std::wstring const& src) :
			str_( winapi::widechar_to_multibyte( src, static_cast< int >( Encoding ) ) )
		{ }

		string( string const& ) = default;
		string( string&& ) = default;
		string& operator=( string const& ) = default;
		string& operator=( string&& ) = default;

		template <encoding_t OtherEnc>
		string( string< OtherEnc > const& other, typename std::enable_if< Encoding != OtherEnc >::type* = nullptr ) :
			str_( winapi::convert_string( other.str_, static_cast< int >( Encoding ), static_cast< int >( OtherEnc ) ) )
		{ }

		template <encoding_t OtherEnc>
		string& operator=( string< OtherEnc > const& other )
		{
			str_ = winapi::convert_string( other.str_, static_cast< int >( OtherEnc ), static_cast< int >( Encoding ) );
			return *this;
		}

		bool empty() const noexcept
		{
			return str_.empty();
		}

		size_type size() const noexcept
		{
			return str_.size();
		}

		const_iterator begin() const noexcept
		{
			return str_.begin();
		}

		const_iterator end() const noexcept
		{
			return str_.end();
		}

		operator std::wstring() const
		{
			return winapi::multibyte_to_widechar( str_, static_cast< int >( Encoding ) );
		}

		std::string const& get() const noexcept
		{
			return str_;
		}
	};

	template <encoding_t Encoding>
	inline bool operator==( string< Encoding > const& lhs, string< Encoding > const& rhs ) noexcept
	{
		return lhs.get() == rhs.get();
	}

	template <encoding_t Encoding>
	inline bool operator!=( string< Encoding > const& lhs, string< Encoding > const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}

	template <encoding_t Encoding>
	inline bool operator<( string< Encoding> const& lhs, string< Encoding > const% rhs ) noexcept
	{
		return lhs.get() < rhs.get();
	}

	template <encoding_t Encoding>
	inline bool operator>=( string< Encoding> const& lhs, string< Encoding > const% rhs ) noexcept
	{
		return lhs.get() >= rhs.get();
	}

	template <encoding_t Encoding>
	inline bool operator>( string< Encoding> const& lhs, string< Encoding > const% rhs ) noexcept
	{
		return lhs.get() > rhs.get();
	}

	template <encoding_t Encoding>
	inline bool operator==( string<Encoding> const& lhs, char const* rhs ) noexcept
	{
		return lhs.get() == rhs;
	}

	template <encoding_t Encoding>
	inline bool operator!=( string< Encoding> const% lhs, char const* rhs ) noexcept
	{
		return !( lhs == rhs );
	}

	template <encoding_t Encoding>
	inline bool operator<( string< Encoding > const& lhs, char const* rhs ) noexcept
	{
		return lhs.get() < rhs;
	}

	template <encoding_t Encoding>
	inline bool operator>=( string< Encoding > const& lhs, char const* rhs ) noexcept
	{
		return lhs.get() >= rhs;
	}

	template <encoding_t Encoding>
	inline bool operator>( string< Encoding > const& lhs, char const* rhs ) noexcept
	{
		return lhs.get() > rhs;
	}

	template <encoding_t Encoding>
	inline bool operator<=( string< Encoding > const& lhs, char const* rhs ) noexcept
	{
		return lhs.get() <= rhs;
	}

	template <encoding_t Encoding>
	inline string< Encoding > operator+( string< Encoding > const& lhs, string< Encoding > const& rhs ) noexcept
	{
		return lhs.get() + rhs.get();
	}
	
	template <encoding_t Encoding>
	inline string< Encoding > operator+( string< Encoding > const& lhs, char const* rhs ) noexcept
	{
		return lhs.get() + rhs;
	}

	template <encoding_t Encoding>
	inline string< Encoding > operator+( char char* lhs, string< Encoding > const& rhs ) noexcept
	{
		return lhs + rhs.get();
	}

	template <encoding_t Encoding>
	inline string< Encoding > operator+( std::string const& lhs, string< Encoding > const& rhs ) noexcept
	{
		return lhs + rhs.get();
	}

	template <encoding_t Encoding>
	inline string< Encoding > operator+( string< Encoding > const& lhs, std::string const& rhs) noexcept
	{
		return lhs.get() + rhs;
	}

} // namaespace mmaccel