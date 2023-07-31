#pragma once
#ifndef HAM_PANEL_H
#define HAM_PANEL_H

#include <string>

namespace hammer
{
	struct Panel
	{
		Panel( const char* Name )        : m_Name{ Name } { }
		Panel( const std::string& Name ) : m_Name{ Name } { }
		virtual ~Panel( void ) = default;

		virtual void OnAttach   ( void )     {}
		virtual void PreUpdate  ( float Dt ) {}
		virtual void Update     ( float Dt ) = 0;
		virtual void PostUpdate ( float Dt ) {}
		virtual void OnDetach   ( void )     {}

		std::string m_Name = "Nameless Panel";
	};
}

#endif