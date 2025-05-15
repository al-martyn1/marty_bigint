/*!
    \file
    \brief Библиотека marty::bigint
    
 */
#pragma once


#include "defs.h"

//
#include <string>
#include <vector>
#include <cstdint>

//
#include "undef_min_max.h"


// Под отладкой вектора гораздо приятнее разглядывать как последовательности чисел, а не символов. А на производительность - наплевать. Алгоритмы отлаживаем на векторах.
#if defined(DEBUG) || defined(_DEBUG)

    #if !defined(MARTY_BIGINT_USE_VECTOR)
        #define MARTY_BIGINT_USE_VECTOR
    #endif

#endif



// #include "marty_bigint/marty_bigint.h"
// marty::
namespace marty {


#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603

    #if (sizeof(int)==1)
    
        using underlying_unsigned_t = std::uint_fast8_t ;
    
    #elif (sizeof(int)==2)
    
        using underlying_unsigned_t = std::uint_fast16_t;
    
    #elif (sizeof(int)==4)
    
        using underlying_unsigned_t = std::uint_fast32_t;
    
    #else
    
        using underlying_unsigned_t = std::uint_fast64_t;
    
    #endif

#else

    #if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__) || defined(__LP64__)
        
        using underlying_unsigned_t = std::uint_fast64_t;
    
    #elif !defined(PLATFORM_BITS) && (defined(_WIN32) || defined(__i386__) || defined(__arm__) || defined(__ILP32__))
        
        using underlying_unsigned_t = std::uint_fast32_t;
    
    #elif !defined(PLATFORM_BITS) && (defined(__MSDOS__) || defined(__DOS__) || defined(__SMALL__))
        
        using underlying_unsigned_t = std::uint_fast16_t ;

    #elif !defined(PLATFORM_BITS) && (defined(__AVR__) || defined(__8051__) || defined(__PIC__))
        
        using underlying_unsigned_t = std::uint_fast8_t;

    #else
        
        using underlying_unsigned_t = unsigned;

    #endif

#endif

using unsigned_t = underlying_unsigned_t;



#ifndef MARTY_BIGINT_USE_VECTOR

    typedef std::basic_string<unsigned_t> number_holder_t;

#else

    typedef std::vector<unsigned_t>       number_holder_t;

#endif






} // namespace marty

// marty::
// #include "marty_bigint/marty_bigint.h"

