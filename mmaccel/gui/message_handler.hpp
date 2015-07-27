#pragma once

#include <functional>

namespace mmaccel
{
	namespace detail
	{
		template <typename... Messages>
		class message_handler_impl;

		template <typename Message, typename... Rest>
		class message_handler_impl< Message, Rest... > :
			public message_handler_impl< Rest... >
		{
			using base_type = message_handler_impl< Rest... >;

			std::function< typename Message::function_type > f_;

		public:
			using base_type::ptr;

			decltype( f_ )* ptr(Message) noexcept
			{
				return &f_;
			}

			decltype( f_ ) const* ptr(Message) const noexcept
			{
				return &f_;
			}
		};

		template <>
		class message_handler_impl<>
		{
		public:
			void ptr() noexcept;
		};

	} // namespace detail

	template <typename... Messages>
	class message_handler :
		detail::message_handler_impl< Messages... >
	{
	public:
		template <typename Message>
		bool empty( Message ) const noexcept
		{
			return !( *this->ptr( Message() ) );
		}

		template <typename Message, typename F>
		void assign( Message, F&& f )
		{
			*this->ptr( Message() ) = std::forward< F >( f );
		}

		template <typename Message, typename... Args>
		auto invoke( Message, Args const&... args ) const
		{
			return ( *this->ptr( Message() ) )( args... );
		}
	};

} // namespace mmaccel