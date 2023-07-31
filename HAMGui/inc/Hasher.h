#pragma once
#ifndef HAM_HASHER_H
#define HAM_HASHER_H

#include "xxHash/xxHash32.h"
#include "xxHash/xxHash64.h"

#include <string>
#include <cstddef>
#include <type_traits>
#include <unordered_map>

namespace hammer
{
	template < size_t HASH_SIZE = 64 >
	class Hasher
	{
	public:

		using HashType = std::conditional_t<HASH_SIZE == 32, XXHash32, XXHash64>;
		using HashSize = std::conditional_t<HASH_SIZE == 32, uint32_t, uint64_t>;

		static Hasher& Instance( void )
		{
			static Hasher<HASH_SIZE> Inst;
			return Inst;
		}

		HashSize Hash( const std::string& Str ) noexcept
		{
			HashSize Result  = m_HashTool.hash( Str.c_str(), Str.length(), 0 );
			auto     It      = m_HashTracking.find( Result );
			bool DoesntExist = (It == m_HashTracking.end()) || (It->second == Str);

			assert( DoesntExist && "Hashing collision has occurred..." );

			m_HashTracking.emplace( Result, Str );
			return Result;
		}

	private:

		Hasher()                         : m_HashTool{ 0 } { }
		~Hasher()                        = default;
		Hasher (const Hasher&)           = delete;
		Hasher& operator=(const Hasher&) = delete;
		
		HashType								     m_HashTool;
		std::unordered_map<uint64_t, std::string>    m_HashTracking;
	};

	/*
		Converts x to string
	*/
	#define  Str(x)     #x
	/*
		32 Bit Hashing
		Valid Types (x): char* / const char* / std::string
	*/
	#define  Hash32(x)  Hasher<32>::Instance().Hash(x)
	/*
		64 Bit Hashing
		Valid Types (x): char* / const char* / std::string
	*/
	#define  Hash64(x)  Hasher<64>::Instance().Hash(x)
}

#endif