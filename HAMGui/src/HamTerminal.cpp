#include "HamTerminal.h"
#include "ConfigReader.h"

#include "imterm/misc.hpp"
#include "imterm/utils.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <array>
#include <optional>
#include <charconv>

// temp
#include <filesystem>


namespace hammer
{
	namespace
	{
		constexpr std::array CommandList
		{
			TerminalCommands::command_type{"clear", "clears the terminal screen", TerminalCommands::Clear, TerminalCommands::InvalidCommand},
			TerminalCommands::command_type{"echo", "prints text", TerminalCommands::Echo, TerminalCommands::InvalidCommand},
			TerminalCommands::command_type{"exit", "closes this terminal", TerminalCommands::Exit, TerminalCommands::InvalidCommand},
			TerminalCommands::command_type{"help", "show this help", TerminalCommands::Help, TerminalCommands::InvalidCommand},
			TerminalCommands::command_type{"quit", "closes this application", TerminalCommands::Quit, TerminalCommands::InvalidCommand},
		};
	}

	TerminalCommands::TerminalCommands()
	{
		for (const command_type& cmd : CommandList)
		{
			add_command_(cmd);
		}
	}

	void TerminalCommands::Clear(argument_type& arg) noexcept
	{
		arg.term.clear();
	}

	void TerminalCommands::Echo(argument_type& arg) noexcept
	{
		if (arg.command_line.size() < 2)
		{
			//arg.term.add_formatted("");
			return;
		}
		if (arg.command_line[1][0] == '-')
		{
			if (arg.command_line[1] == "--help" || arg.command_line[1] == "-help")
			{
				//arg.term.add_formatted("usage: {} [text to be printed]", arg.command_line[0]);
			}
			else
			{
				//arg.term.add_formatted_err("Unknown argument: {}", arg.command_line[1]);
			}
		}
		else
		{
			std::string str{};
			auto it = std::next(arg.command_line.begin(), 1);
			while (it != arg.command_line.end() && it->empty())
			{
				++it;
			}
			if (it != arg.command_line.end())
			{
				str = *it;
				for (++it; it != arg.command_line.end(); ++it)
				{
					if (it->empty())
					{
						continue;
					}
					str.reserve(str.size() + it->size() + 1);
					str += ' ';
					str += *it;
				}
			}
			//arg.term.add_formatted("{}", str);
		}
	}

	void TerminalCommands::Exit(argument_type& arg) noexcept
	{
		arg.term.set_should_close();
	}

	void TerminalCommands::Help(argument_type& arg) noexcept
	{
		constexpr unsigned long list_element_name_max_size = misc::max_size(CommandList.begin(), CommandList.end(),
			[](const command_type& cmd) { return cmd.name.size(); });

		//arg.term.add_formatted("Available commands:");
		for (const command_type& cmd : CommandList)
		{
			//arg.term.add_formatted("        {:{}} | {}", cmd.name, list_element_name_max_size, cmd.description);
		}
		//arg.term.add_formatted("");
		//arg.term.add_formatted("Additional information might be available using \"'command' --help\"");
	}

	void TerminalCommands::Quit(argument_type& arg) noexcept
	{
		arg.val.m_bClose = true;
	}

	TerminalLoggerManager::TerminalLoggerManager() :
		m_TerminalCommands{ }
		, m_Terminal{ m_TerminalCommands }
		, m_Logger { std::make_shared<spdlog::logger>("Logger") }
	{

	}

	void TerminalLoggerManager::InitializeTerminal( const std::string& ConfigPath )
	{
		// Open & read file
		ConfigReader Reader( ConfigPath );

		// Retrieve Config
		auto EnableTerminalResizing = Reader.GetVal<bool>                     ( "Terminal", "enable_terminal_resizing" );
		auto TerminalLogLevel       = Reader.GetVal<spdlog::level::level_enum>( "Terminal", "terminal_log_level" );

		auto EnableConsoleMT        = Reader.GetVal<bool>                     ( "Writer", "enable_mt" );                          // write extra condition later
		auto ConsoleLogLevel        = Reader.GetVal<spdlog::level::level_enum>( "Writer", "console_log_level" );

		auto EnableLoggerMT         = Reader.GetVal<bool>                     ( "Logger", "enable_mt" );                          // write extra condition later
		auto LoggerLogLevel         = Reader.GetVal<spdlog::level::level_enum>( "Logger", "console_log_level" );
		auto LoggerLogName          = Reader.GetVal<std::string>              ( "Logger", "log_name" );


		m_Terminal.set_autocomplete_pos( ImTerm::position::up );

		// Disable terminal resizing
		if ( !EnableTerminalResizing )
		{
			m_Terminal.disallow_x_resize();
			m_Terminal.disallow_y_resize();
		}

		// Set terminal log level
		m_Terminal.get_terminal_helper()->set_level(TerminalLogLevel);

		// Create console sink
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level( ConsoleLogLevel );

		// Create logger sink
		auto logger_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>( CURRENT_FOLDER_PATH + "\\logs\\" + LoggerLogName, true );
		logger_sink->set_level( LoggerLogLevel );

		// Construct a "shared logger" - ImTerm terminal + Console writer + Console logger
		m_Logger = std::make_shared<spdlog::logger>( spdlog::logger("Ham Logs", { m_Terminal.get_terminal_helper(), console_sink, logger_sink }) );

		// Initialize default spdlog logger - For linking all output to terminal
		spdlog::set_default_logger( m_Logger );
		spdlog::set_level( spdlog::level::trace );
	}

	void TerminalLoggerManager::ShowTerminal( void )
	{
		m_Terminal.show();
	}
}