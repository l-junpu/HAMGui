#include "ConfigReader.h"

namespace hammer
{
	ConfigReader::ConfigReader( const std::string& ConfigFilePath ) noexcept :
		m_ConfigFile{ ConfigFilePath }
	{
		m_ConfigFile.read( m_Ini );
	}

	mINI::INIStructure& ConfigReader::GetFile(void) noexcept
	{
		return m_Ini;
	}

	spdlog::level::level_enum ConfigReader::StringToConfig( const std::string& Level ) const noexcept
	{
		if      ( Level == "trace")    return spdlog::level::trace;
		else if ( Level == "debug")    return spdlog::level::debug;
		else if ( Level == "info")     return spdlog::level::info;
		else if ( Level == "warn")     return spdlog::level::warn;
		else if ( Level == "err")      return spdlog::level::err;
		else if ( Level == "critical") return spdlog::level::critical;
	}
}