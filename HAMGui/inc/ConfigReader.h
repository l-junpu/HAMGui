#pragma once
#ifndef HAM_CONFIG_READER_H
#define HAM_CONFIG_READER_H

#include <string>
#include <mini/ini.h>
#include <imterm/utils.hpp>
#include <spdlog/spdlog.h>

namespace hammer
{
	class ConfigReader
	{
	public:

		ConfigReader( const std::string& ConfigFilePath ) noexcept;

		mINI::INIStructure& GetFile( void ) noexcept;

		template < typename T >
		T GetVal( const std::string& Section
				, const std::string& Key ) const noexcept;

		// has
			// section
			// key


		// size

	private:

		spdlog::level::level_enum StringToConfig( const std::string& ) const noexcept;

		mINI::INIFile		m_ConfigFile;
		mINI::INIStructure  m_Ini;
	};























	template<typename T>
	inline T ConfigReader::GetVal( const std::string& Section, const std::string& Key ) const noexcept
	{
		if constexpr ( std::is_floating_point_v<T> )
		{
			return static_cast<T>( std::stod( m_Ini.get(Section).get(Key) ) );
		}
		else if constexpr ( std::is_same_v<T, bool> )
		{
			return m_Ini.get(Section).get(Key) == "true" ? true : false;
		}
		else if constexpr ( std::is_same_v<T, spdlog::level::level_enum> )
		{
			return StringToConfig( m_Ini.get(Section).get(Key) );
		}
		else if constexpr ( std::is_integral_v<T> )
		{
			return static_cast<T>( std::stol( m_Ini.get(Section).get(Key) ) );
		}
		else if constexpr ( std::is_same_v<T, std::string> )
		{
			return m_Ini.get(Section).get(Key);
		}
	}
}

#endif