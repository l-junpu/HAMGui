#pragma once
#ifndef HAM_EVENT_MANAGER_H
#define HAM_EVENT_MANAGER_H

#include "Event.h"
#include "Hasher.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace hammer
{
	class EventManager
	{
	public:

		static EventManager& Instance( void )
		{
			static EventManager Inst;
			return Inst;
		}

		template < typename T_EVENT
				 , auto     T_FUNCTION >
		void RegisterStandaloneEvent( void ) noexcept;

		template < typename T_EVENT
				 , auto     T_FUNCTION
				 , typename T_HOST >
		void RegisterClassEvent( T_HOST* Host ) noexcept;

		template < typename    T_EVENT
				 , typename... T_ARGS >
		void BroadcastEvent( T_ARGS&&... Args ) const noexcept;

	private:

		EventManager()                               = default;
		~EventManager()                              = default;
		EventManager(const EventManager&)            = delete;
		EventManager& operator=(const EventManager&) = delete;

		std::vector<std::unique_ptr<EventInterface>>  m_StandaloneEventsList;
		std::unordered_map<uint64_t, EventInterface*> m_StandaloneEventsMap;

		std::vector<std::unique_ptr<EventInterface>>  m_ClassEventsList;
		std::unordered_map<uint64_t, EventInterface*> m_ClassEventsMap;
	};


	#define  EventMgr  EventManager::Instance()

















	template < typename T_EVENT
			 , auto     T_FUNCTION >
	inline void EventManager::RegisterStandaloneEvent( void ) noexcept
	{
		auto Hashed_EventName = Hash64( Str(T_EVENT) );
		auto It               = m_StandaloneEventsMap.find( Hashed_EventName );

		if ( It == m_StandaloneEventsMap.end() )
		{
			m_StandaloneEventsList.push_back( std::make_unique<T_EVENT>() );
			m_StandaloneEventsMap.emplace( Hashed_EventName, m_StandaloneEventsList.back().get() );
			It = m_StandaloneEventsMap.find( Hashed_EventName );
		}

		static_cast<T_EVENT*>( It->second )->RegisterEvent<T_FUNCTION>();
	}

	template < typename T_EVENT
			 , auto     T_FUNCTION
			 , typename T_HOST >
	inline void EventManager::RegisterClassEvent( T_HOST* Host ) noexcept
	{
		auto Hashed_EventName = Hash64( Str(T_EVENT) );
		auto It               = m_ClassEventsMap.find( Hashed_EventName );

		if ( It == m_ClassEventsMap.end() )
		{
			m_ClassEventsList.push_back( std::make_unique<T_EVENT>() );
			m_ClassEventsMap.emplace( Hashed_EventName, m_ClassEventsList.back().get() );
			It = m_ClassEventsMap.find( Hashed_EventName );
		}

		static_cast<T_EVENT*>( It->second )->RegisterEvent<T_FUNCTION>( Host );
	}

	template < typename    T_EVENT
			 , typename... T_ARGS >
	inline void EventManager::BroadcastEvent( T_ARGS&&... Args ) const noexcept
	{
		auto Hashed_EventName = Hash64( Str(T_EVENT) );

		if ( T_EVENT::Type == EventType::STANDALONE_EVENT )
		{
			auto It = m_StandaloneEventsMap.find( Hashed_EventName );
			assert( It != m_StandaloneEventsMap.end() && "Event has yet to be registered..." );
			static_cast<T_EVENT*>( It->second )->BroadcastEvent( std::forward<T_ARGS&&>( Args )... );
		}
		else if ( T_EVENT::Type == EventType::CLASS_EVENT )
		{
			auto It = m_ClassEventsMap.find( Hashed_EventName );
			assert( It != m_ClassEventsMap.end() && "Event has yet to be registered..." );
			static_cast<T_EVENT*>( It->second )->BroadcastEvent( std::forward<T_ARGS&&>( Args )... );
		}
	}
}

#endif