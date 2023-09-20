#pragma once
#ifndef HAM_IMTERM_H
#define HAM_IMTERM_H

#include <spdlog/spdlog.h>
#include <imterm/terminal.hpp>
#include <imterm/terminal_helpers.hpp>

#define  CURRENT_FOLDER_PATH  std::filesystem::current_path().string()
#define  PARENT_FOLDER_PATH   std::filesystem::current_path().parent_path().string()

namespace hammer
{
	struct CustomCommands
	{
		bool m_bClose = false;
	};

	class TerminalCommands : public ImTerm::basic_spdlog_terminal_helper< TerminalCommands
		                                                                , CustomCommands
		                                                                , misc::no_mutex >
	{
	public:

		TerminalCommands();

		static std::vector<std::string> InvalidCommand(argument_type&) { return {}; }

		static void Clear(argument_type&) noexcept;
		static void Echo (argument_type&) noexcept;
		static void Exit (argument_type&) noexcept;
		static void Help (argument_type&) noexcept;
		static void Quit (argument_type&) noexcept;
	};

	class TerminalLoggerManager
	{
	public:

		TerminalLoggerManager();
		void InitializeTerminal( const std::string& ConfigPath );
		void ShowTerminal      ( void );

	private:

		hammer::CustomCommands				  m_TerminalCommands = {};
		ImTerm::terminal<TerminalCommands>    m_Terminal;
		std::shared_ptr<spdlog::logger>       m_Logger;
	};
}


#endif