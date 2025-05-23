/*!
    \file
    \brief Типы для marty::BigInt
 */
#pragma once

#include "defs.h"

//
#include <cstdint>
#include <type_traits>
#include <limits>
#include <typeinfo>

//
#include "undef_min_max.h"


// #include "marty_bigint/marty_bigint.h"
// marty::bigint_utils::
namespace marty {
namespace bigint_utils {


//----------------------------------------------------------------------------
template < typename T
         , std::enable_if_t< std::is_integral_v<T>, int> = 0
         >
constexpr
std::make_unsigned_t<T> toUnsignedCast(T t)
{
   using UT = std::make_unsigned_t<T>;
   return static_cast<UT>(t);
}

//----------------------------------------------------------------------------
template < typename T
         , std::enable_if_t< std::is_integral_v<T>, int> = 0
         >
constexpr
std::make_signed_t<T> toSignedCast(T t)
{
   using UT = std::make_signed_t<T>;
   return static_cast<UT>(t);
}

//----------------------------------------------------------------------------
template < typename T
         , std::enable_if_t< std::is_integral_v<T> and not std::is_signed_v<T>, int> = 0
         >
constexpr
std::make_unsigned_t<T> toUnsignedAbs(T t)
{
   using UT = std::make_unsigned_t<T>;
   return static_cast<UT>(t);
}

template < typename T
         , std::enable_if_t< std::is_integral_v<T> and std::is_signed_v<T>, int> = 0
         >
constexpr
std::make_unsigned_t<T> toUnsignedAbs(T t)
{
   using UT = std::make_unsigned_t<T>;
   // return static_cast<UT>(t);
   return (t<0) ? static_cast<UT>(static_cast<T>(-(t + static_cast<T>(1))) + static_cast<T>(1)) : (static_cast<UT>(t));
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename IntType> constexpr
IntType makeHighBitsMask1();
// {
//     //static_assert(std::is_integral_v<T>, "T must be an integral type");
//     static_assert(false, "makeHighBitsMask1 not implemented for this type");
//     return 0;
// }

#define MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(t, v) \
            template<> constexpr                        \
            t makeHighBitsMask1<t>()                    \
            {                                           \
                static_assert(std::is_integral_v<t>, "makeHighBitsMask1: T must be an integral type"); \
                return static_cast<t>(v);               \
            }

MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::int8_t   , 0x80               )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::uint8_t  , 0x80u              )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::int16_t  , 0x8000             )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::uint16_t , 0x8000u            )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::int32_t  , 0x80000000         )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::uint32_t , 0x80000000u        )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::int64_t  , 0x8000000000000000 )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK1_IMPL(std::uint64_t , 0x8000000000000000u)

//----------------------------------------------------------------------------
template<typename IntType> constexpr
IntType makeHighBitsMask4();
// {
//     //static_assert(std::is_integral_v<T>, "T must be an integral type");
//     static_assert(false, "makeHighBitsMask1 not implemented for this type");
//     return 0;
// }

#define MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(t, v) \
            template<> constexpr                        \
            t makeHighBitsMask4<t>()                    \
            {                                           \
                static_assert(std::is_integral_v<t>, "makeHighBitsMask4: T must be an integral type"); \
                return static_cast<t>(v);               \
            }

MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::int8_t   , 0xF0               )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::uint8_t  , 0xF0u              )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::int16_t  , 0xF000             )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::uint16_t , 0xF000u            )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::int32_t  , 0xF0000000         )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::uint32_t , 0xF0000000u        )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::int64_t  , 0xF000000000000000 )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK4_IMPL(std::uint64_t , 0xF000000000000000u)

//----------------------------------------------------------------------------
template<typename IntType> constexpr
IntType makeHighBitsMask8();
// {
//     //static_assert(std::is_integral_v<T>, "T must be an integral type");
//     static_assert(false, "makeHighBitsMask8 not implemented for this type");
//     return 0;
// }

#define MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(t, v) \
            template<> constexpr                        \
            t makeHighBitsMask8<t>()                    \
            {                                           \
                static_assert(std::is_integral_v<t>, "makeHighBitsMask8: T must be an integral type"); \
                return static_cast<t>(v);               \
            }

MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::int8_t   , 0xFF               )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::uint8_t  , 0xFFu              )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::int16_t  , 0xFF00             )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::uint16_t , 0xFF00u            )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::int32_t  , 0xFF000000         )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::uint32_t , 0xFF000000u        )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::int64_t  , 0xFF00000000000000 )
MARTY_BIGINT_UTILS_MAKEHIGHBITSMASK8_IMPL(std::uint64_t , 0xFF00000000000000u)

//----------------------------------------------------------------------------


template<std::size_t size, typename IntType> constexpr
IntType makeHighBitsMask()
{
    static_assert(size == 1 || size == 4 || size == 8  /* || size == 8 */ , "makeHighBitsMask: unsupported size");

    return size==1 ? makeHighBitsMask1<IntType>()
                   : size==4 ? makeHighBitsMask4<IntType>()
                             : makeHighBitsMask8<IntType>()
                   ;
}

template<std::size_t size, typename IntType> constexpr
IntType makeLowBitsMask()
{
    static_assert(size == 1 || size == 4 || size == 8  /* || size == 8 */ , "makeHighBitsMask: unsupported size");

    return static_cast<IntType>(
        size==1 ? 1u
                : size==4 ? 0x0Fu
                          : 0xFFu
                         );
}

//----------------------------------------------------------------------------
// Для 8ми бит если надо сдвинуть 1 старший бит в младший - сдвиг на 7
// Для 16ти бит если надо сдвинуть 1 старший бит в младший - сдвиг на 15
// Для 8ми бит если надо сдвинуть 4 старших бит в младший - сдвиг на 4, для 16ти - 12
// Для 8ми бит если надо сдвинуть 8 старших бит в младший - сдвиг на 0, для 16ти - 8
// Итого: всего бит в слове минус количество значимых бит
template<std::size_t size, typename IntType>
IntType shiftBitsToLow(IntType val)
{
    static_assert(size == 1 || size == 4 || size == 8  /* || size == 8 */ , "shiftBitsToLow: unsupported size");

    return static_cast<IntType>( val >> ((sizeof(IntType)*CHAR_BIT)-size) );
}

template<std::size_t size, typename IntType>
IntType shiftBitsToHigh(IntType val)
{
    static_assert(size == 1 || size == 4 || size == 8  /* || size == 8 */ , "shiftBitsToLow: unsupported size");

    return static_cast<IntType>( val << ((sizeof(IntType)*CHAR_BIT)-size) );
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template < typename T
         , std::enable_if_t< std::is_integral_v<T>, int> = 0
         >
constexpr
int getTypeDecimalDigits()
{
    return int(std::numeric_limits<T>::digits10);
}

inline
unsigned getPower10(int p)
{
    unsigned res = 1;
    for(; p>0; --p)
       res *= 10;

    return res;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace bigint_utils
} // namespace marty

// marty::bigint_utils::
// #include "marty_bigint/marty_bigint.h"
