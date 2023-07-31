#pragma once
#ifndef HAM_APP_H
#define HAM_APP_H

#include "Panel.h"
#include "LocalClock.h"

#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>
#include <functional>

namespace hammer
{
	struct ApplicationSpecs
	{
		std::string  m_Name	  = "Default Application Name";
		uint32_t	 m_Width  = 1600;
		uint32_t	 m_Height = 900;
	};

	class Application
	{
	public:

		// Constructors
		Application( const ApplicationSpecs&  Specs );

		Application( const std::string&       WinName
				   , uint32_t                 Width
				   , uint32_t                 Height ); // allow fps customizations

		~Application( void );


		template < typename    T_Panel
			     , typename... T_ARGS >
		void AddPanel( const char*            PanelName
					 , T_ARGS&&...            Args );
		void RemovePanel( const char*         PanelName ) noexcept;

		void Loop( void ) noexcept;


	private:

		void InitializeTheme ( void );
		int  InitializeWindow( void );
		void TerminateWindow ( void );
		bool PanelExists( const std::string&  PanelName );


		bool                                  m_Running             = true;
		Clock								  m_Clock               = {};
		ApplicationSpecs					  m_Specs               = {};
		GLFWwindow*		                      m_pApplication        = nullptr;
		std::function<void()>                 m_ApplicationCallback = {};
		std::vector<std::shared_ptr<Panel>>   m_PanelList           = {};
		std::unordered_map<std::string, int>  m_PanelMapping        = {};
	};




	template < typename    T_Panel
			 , typename... T_ARGS >
	inline void Application::AddPanel( const char* PanelName
		                             , T_ARGS&&... Args )
	{
		static_assert( std::is_base_of_v<hammer::Panel, T_Panel>					// Can replace this with concepts later or keep this to potentially downgrade
						, "Verify that T_Pannel is derived from hammer::Panel");
		assert( !PanelExists( PanelName )
			 && "Panel has already been added");

		auto NewPanel = std::make_shared<T_Panel>( PanelName, std::forward<T_ARGS&&>(Args)... );
			
		m_PanelList.push_back( NewPanel );
		NewPanel->OnAttach();
		m_PanelMapping.emplace( PanelName, (m_PanelList.size() - 1) );
	}
}

#endif