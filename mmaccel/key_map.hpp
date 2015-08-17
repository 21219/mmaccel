#pragma once

#include "platform.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <boost/spirit/include/qi.hpp>
#include "mmd_map.hpp"
#include "keyboard.hpp"
#include "handler.hpp"

namespace mmaccel
{
	using key_map_t = boost::container::flat_map< std::string, keys_combination >;

	inline void save_key_map( boost::string_ref path, key_map_t const& km, json::data_type const& mm )
	{
		std::ofstream ofs( path.data() );
		if( !ofs ) {
			return;
		}

		int c_index = 0;
		for( auto const& c : mmd_map::get_category( mm ) ) {
			ofs << u8"##### " << c.first << u8" #####\n";

			int sub_index = 0;
			for( auto const& sub : mmd_map::get_sub_category( mm, c_index ) ) {
				ofs << u8"### " << sub.first << u8" ###\n";

				auto const elem = mmd_map::get_elements( mm, c_index, sub_index );
				for( auto const& e : elem ) {
					ofs << u8"# " << mmd_map::get_friendly_name( e ) << u8"\n";
					ofs << e.first << u8" = ";

					auto itr = km.find( e.first );
					if( itr != km.end() ) {
						ofs << keys_to_string( itr->second );
					}

					ofs << u8"\n";
				}

				ofs << u8"\n";
				++sub_index;
			}

			ofs << u8"\n";
			++c_index;
		}
	}

	inline key_map_t load_key_map( boost::string_ref path )
	{
		namespace qi = boost::spirit::qi;

		std::ifstream ifs( path.data() );
		if( !ifs ) {
			return{};
		}

		auto const rule_comment = boost::proto::deep_copy( qi::lit( '#' ) >> qi::char_ );
		auto const rule_map = boost::proto::deep_copy( +qi::alnum >> '=' >> -( +qi::char_ ) );

		key_map_t keys;
		std::string line;
		while( std::getline( ifs, line ) ) {
			if( qi::phrase_parse( line.begin(), line.end(), rule_comment, qi::unicode::space ) ) {
				continue;
			}

			std::string name;
			boost::optional< std::string > keys_str;
			if( !qi::phrase_parse( line.begin(), line.end(), rule_map, qi::unicode::space, name, keys_str ) ) {
				continue;
			}
			if( !keys_str ) {
				continue;
			}

			keys.emplace( name, string_to_keys( *keys_str ) );
		}

		return keys;
	}

	using key_handler_map_t = std::unordered_map< keys_combination, std::function< void( keyboard_state& ) > >;

	inline key_handler_map_t load_key_handler_map( boost::string_ref path, json::data_type const& mm, HWND mmd )
	{
		namespace qi = boost::spirit::qi;

		std::ifstream ifs( path.data() );
		if( !ifs ) {
			return{};
		}

		auto const rule_comment = boost::proto::deep_copy( qi::lit( '#' ) >> qi::char_ );
		auto const rule_map = boost::proto::deep_copy( +qi::alnum >> '=' >> -( +qi::char_ ) );

		auto const elems = mmd_map::get_elements_map( mm );

		key_handler_map_t khm;
		std::string line;

		while( std::getline( ifs, line ) ) {
			if( qi::phrase_parse( line.begin(), line.end(), rule_comment, qi::unicode::space ) ) {
				continue;
			}

			std::string name;
			boost::optional< std::string > keys_str;
			if( !qi::phrase_parse( line.begin(), line.end(), rule_map, qi::unicode::space, name, keys_str ) ) {
				continue;
			}
			if( !keys_str ) {
				continue;
			}

			auto const elem_itr = elems.find( name );
			khm.emplace( string_to_keys( *keys_str ), make_handler( *elem_itr, mmd ) );
		}

		return khm;
	}

} // namespace mmaccel