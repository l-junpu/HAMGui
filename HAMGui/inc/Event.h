#pragma once
#ifndef HAM_EVENT_H
#define HAM_EVENT_H

#include <vector>
#include <cstdint>

namespace hammer
{
	enum class EventType : uint8_t
	{
		CLASS_EVENT = 0
	,	STANDALONE_EVENT
	};


	struct EventInterface
	{
		virtual ~EventInterface() = default;
	};


	template < typename... T_ARGS >
	class ClassEvent : public EventInterface
	{
	public:

		static constexpr auto Type = EventType::CLASS_EVENT;

		struct Info
		{
			using Callback = void( void*, T_ARGS&&... );

			void*		m_pHost;
			Callback*	m_pCallback;
		};

		ClassEvent( void ) noexcept = default;
		ClassEvent( const ClassEvent& ) noexcept = delete;
		virtual ~ClassEvent() = default;

		template < auto		T_FUNCTION
				 , typename T_HOST >
		void RegisterEvent( T_HOST* System ) noexcept;

		//template < typename T_HOST >
		//void RemoveEvent( T_HOST* System ) noexcept;

		void BroadcastEvent( T_ARGS&&... Args ) const noexcept;

	private:

		std::vector<ClassEvent::Info> m_Events;
	};


	template < typename... T_ARGS >
	class StandaloneEvent : public EventInterface
	{
	public:

		static constexpr auto Type = EventType::STANDALONE_EVENT;

		struct Info
		{
			using Callback = void( T_ARGS&&... );

			Callback*	m_pCallback;
		};

		StandaloneEvent( void ) noexcept = default;
		StandaloneEvent( const StandaloneEvent& ) noexcept = delete;
		virtual ~StandaloneEvent() = default;

		template < auto T_FUNCTION >
		void RegisterEvent( void ) noexcept;

		// Not sure how I should implement this for now
		//template < auto T_FUNCTION >
		//void RemoveEvent( void ) noexcept;

		void BroadcastEvent( T_ARGS&&... Args ) const noexcept;

	private:

		std::vector<StandaloneEvent::Info> m_Events;
	};














	template < typename... T_ARGS >
	template < auto		T_FUNCTION
			 , typename T_HOST >
	inline void ClassEvent<T_ARGS...>::RegisterEvent( T_HOST* Host ) noexcept
	{
		m_Events.push_back( Info
							{
								.m_pHost     = Host
							,	.m_pCallback = []( void* FHost, T_ARGS&&... Args ) constexpr noexcept
											   {
											       std::invoke( T_FUNCTION, reinterpret_cast<T_HOST*>( FHost ), std::forward<T_ARGS&&>( Args )... );
											   }
							} );
	}

	template < typename... T_ARGS >
	inline void ClassEvent<T_ARGS...>::BroadcastEvent( T_ARGS&&... Args ) const noexcept
	{
		for ( const auto& Event : m_Events )
		{
			Event.m_pCallback( Event.m_pHost, std::forward<T_ARGS&&>( Args )... );
		}
	}


	template < typename... T_ARGS >
	template < auto T_FUNCTION >
	inline void StandaloneEvent<T_ARGS...>::RegisterEvent( void ) noexcept
	{
		m_Events.push_back( Info
							{
								.m_pCallback = []( T_ARGS&&... Args ) constexpr noexcept
											   {
											       std::invoke( T_FUNCTION, std::forward<T_ARGS&&>( Args )... );
											   }
							} );
	}

	template < typename... T_ARGS >
	inline void StandaloneEvent<T_ARGS...>::BroadcastEvent( T_ARGS&&... Args ) const noexcept
	{
		for ( const auto& Event : m_Events )
		{
			Event.m_pCallback( std::forward<T_ARGS&&>( Args )... );
		}
	}
}

#endif