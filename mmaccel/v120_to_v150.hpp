#pragma once

#include "platform.hpp"
#include "key_map.hpp"
#include "winapi/message_box.hpp"
#include "winapi/path.hpp"
#include <fstream>

namespace mmaccel {

	inline void mmaccel_txt_to_key_map_txt(boost::string_ref ma_txt, boost::string_ref km_txt, json::data_type const& mm)
	{
		if( !winapi::path_file_exists( ma_txt ) ) {
			return;
		}
		if( winapi::path_file_exists( km_txt ) ) {
			return;
		}

		auto km = load_key_map( ma_txt );

		auto itr = km.find( u8"MenuMorphLipSink" );
		if( itr != km.end() ) {
			km.emplace( std::string( u8"MenuMorphLipSync" ), itr->second );
			km.erase( itr );
		}

		save_key_map( km_txt, km, mm );

		winapi::message_box( u8"MMAccel", u8"mmaccel.txtから設定を引き継ぎました。", MB_OK );
	}

} // namespace mmaccel