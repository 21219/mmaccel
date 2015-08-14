#pragma once

#include "key_handler.hpp"
#include "keyboard.hpp"
#include "mmd_map.hpp"
#include "winapi/string.hpp"
#include <boost/utility/string_ref.hpp>
#include <fstream>

namespace mmaccel
{
	class key_map
	{
		std::vector< std::pair< keys_combination, key_handler > > map_;

	public:
		using value_type = decltype( map_ )::value_type;
		using size_type = decltype( map_ )::size_type;
		using iterator = decltype( map_ )::iterator;
		using const_iterator = decltype( map_ )::const_iterator;

		key_map() = default;
		key_map( key_map const& ) = default;
		key_map( key_map&& ) = default;
		key_map& operator=( key_map const& ) = default;
		key_map& operator=( key_map&& ) = default;

		bool empty() const noexcept
		{
			return map_.empty();
		}

		size_type size() const noexcept
		{
			return map_.size();
		}

		iterator begin() noexcept
		{
			return map_.begin();
		}

		iterator end() noexcept
		{
			return map_.end();
		}

		const_iterator begin() const noexcept
		{
			return map_.begin();
		}

		const_iterator end() const noexcept
		{
			return map_.end();
		}

		iterator find( boost::string_ref name ) noexcept
		{
			return boost::find_if( map_, [this, &name]( value_type const& v ) { return v.second.name == name; } );
		}

		const_iterator find( boost::string_ref name ) const noexcept
		{
			return boost::find_if( map_, [this, &name]( value_type const& v ) { return v.second.name == name; } );
		}

		void insert( keys_combination&& k, key_handler const& kh )
		{
			map_.emplace_back( std::move( k ), kh );
			boost::stable_sort( map_, []( value_type const& lhs, value_type const& rhs ) { return ( lhs.first.size() > rhs.first.size() ) || ( lhs.first < rhs.first ); } );
		}

		void erase( iterator itr )
		{
			map_.erase( itr );
		}
	};

	namespace detail
	{
		template <typename F>
		inline void write_key_map_impl( boost::string_ref path, json::data_type const& mm, F&& f )
		{
			std::ofstream ofs( path.data() );
			if( !ofs ) {
				return;
			}

			auto const& category = mmd_map::get_category( mm );
			std::size_t c_index = 0;
			for( auto const& c : category ) {
				ofs << "##### " << c.first << " #####\n";

				auto const& sub_category = mmd_map::get_sub_category( mm, c_index );
				std::size_t sc_index = 0;
				for( auto const& sc : sub_category ) {
					ofs << "### " << sc.first << " ###\n";

					auto const& elements = mmd_map::get_elements( mm, c_index, sc_index );
					for( auto const& elem : elements ) {
						ofs << "# " << mmd_map::get_friendly_name( elem ) << "\n";
						ofs << elem.first << "=" << f( elem.first ) << "\n";
					}

					ofs << "\n";
					++sc_index;
				}

				ofs << "\n";
				++c_index;
			}
		}

	} // namespace detail

	inline void write_key_map( boost::string_ref path, json::data_type const& mm)
	{
		detail::write_key_map_impl( path, mm, []( boost::string_ref ) -> std::string { return ""; } );
	}

	inline void write_key_map( boost::string_ref path, json::data_type const& mm, key_map const& km )
	{
		detail::write_key_map_impl( path, mm, [&km]( boost::string_ref name ) -> std::string {
			auto itr = km.find( name );
			if( itr != km.end() ) {
				return keys_to_string( itr->first );
			}

			return "";
		} );
	}

} // namespace mmaccel