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
// marty::bigint_details::
namespace marty {
namespace bigint_details {


#if defined(MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE)

    using underlying_unsigned_t = MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE;

#else

    // Мы храним модуль числа в релизе не в векторе, а в std::string
    // В std::string применяется small string optimization оптимизация
    // Обычно буфер, хранящийся в строке - 16 байт, поэтому без аллокации
    // обходятся строки до 15ти char'ов (+1 байт на завершающий ноль),
    // или строки до 7ти wchar_t'ов (под виндой, sizeof(wchar_t)==2).

    // По идее, в 16 байт влезает два 64битных целых - одно будет само значение,
    // второе - завершающий 0, но это не точно - можем и не вписаться по какой-либо причине.

    // Если мы сложим два 64битных целых - то можем уже вылезти за два 64битных целых
    // и будет аллокация.

    // Но если мы будем хранить BigInt чанками по 32 бита - то аллокации будут происходить
    // позже, по крайней мере, для '+'/'-'

    // Но, скорее всего, если использовать чанки по 32 бита, то все операции будут несколько медленнее.
    // Вопрос в том, насколько медленнее, и где мы больше потеряем - на возможных лишних аллокациях
    // для больших значений, или на более медленном исполнении при использовании более мелких чанков?

    // Надо тестировать, а пока ограничим размер чанка 32мя битами.


    #if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
    
        #if (sizeof(int)==1)
        
            using underlying_unsigned_t = std::uint_fast8_t ;
        
        #elif (sizeof(int)==2)
        
            using underlying_unsigned_t = std::uint_fast16_t;
        
        #elif (sizeof(int)==4)
        
            using underlying_unsigned_t = std::uint_fast32_t;
        
        #else
        
            using underlying_unsigned_t = std::uint_fast32_t;
        
        #endif
    
    #else
    
        #if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__) || defined(__LP64__)
            
            using underlying_unsigned_t = std::uint_fast32_t;
        
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

#endif


using unsigned_t = underlying_unsigned_t;



#ifndef MARTY_BIGINT_USE_VECTOR

    typedef std::basic_string<unsigned_t> number_holder_t;

#else

    typedef std::vector<unsigned_t>       number_holder_t;

#endif





} // namespace bigint_details
} // namespace marty

// marty::bigint_details::
// #include "marty_bigint/marty_bigint.h"

