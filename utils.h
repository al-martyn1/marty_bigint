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
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
constexpr int getTypeDecimalDigits()
{
    return int(std::numeric_limits<T>::digits10);
}

inline unsigned getPower10(int p)
{
    unsigned res = 1;
    for(; p>0; --p)
       res *= 10;

    return res;
}

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
constexpr int getTypeOctalDigits()
{
    return (sizeof(T) * CHAR_BIT) / 3;
}

inline unsigned getPower8(int p)
{
    unsigned res = 1;
    for(; p>0; --p)
       res *= 8;

    return res;
}


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace details{

inline constexpr char digitToCharDec(int d)
{
    return char('0'+d);
}

inline constexpr char digitToCharAlpha(int d, bool bUpper)
{
    return char((bUpper?'A':'a')+d);
}

// constexpr
// inline
// std::size_t calcExpandSize(std::size_t curSize, std::size_t requiredSize)
// {
//     return curSize>requiredSize ? 0 : requiredSize-curSize;
// }
//  
// inline
// std::string makeStrRepitition(std::size_t n, const std::string &str)
// {
//     std::string res; res.reserve(str.size()*n);
//     for(std::size_t i=0; i!=n; ++i)
//         res.append(str);
//     return res;
// }

} // namespace details

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
inline constexpr int toDigit(char ch)
{
    return (ch>='0' && ch<='9')
           ? int(ch-'0')
           : (ch>='a' && ch<='f')
             ? int(ch-'a') + 10
             : (ch>='A' && ch<='F')
               ? int(ch-'A') + 10
               : -1
           ;
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
constexpr int toDigit(T ch)
{
    return toDigit(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr bool isSpace(char ch)
{
    return ch==' ';
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
inline constexpr bool isSpace(T ch)
{
    return isSpace(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr bool isSign(char ch)
{
    return ch=='-' || ch=='+';
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
inline constexpr bool isSign(T ch)
{
    return isSign(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr int toSign(char ch)
{
    return ch=='-'
           ? -1
           : ch=='+'
             ? +1
             : 0
           ;
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
inline constexpr int toSign(T ch)
{
    return toSign(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr bool isGroupSep(char ch)
{
    return ch=='\'' || ch=='_';
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
inline constexpr bool isGroupSep(T ch)
{
    return isGroupSep(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr bool isBase(char ch)
{
    return ch=='x' || ch=='X' || ch=='b' || ch=='B';
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
inline constexpr bool isBase(T ch)
{
    return isBase(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr int toBase(char ch)
{
    return ch=='x' || ch=='X'
           ? 16
           : ch=='b' || ch=='B'
             ? 2
             : 0
           ;
}

//----------------------------------------------------------------------------
template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
inline constexpr int toBase(T ch)
{
    return toSign(char(ch));
}

//----------------------------------------------------------------------------
inline constexpr char digitToChar(int d, bool bUpper)
{
    return d<10 ? details::digitToCharDec(d) : details::digitToCharAlpha(d-10, bUpper);
}

//----------------------------------------------------------------------------
inline constexpr char digitToChar(unsigned d, bool bUpper)
{
    return d<10 ? details::digitToCharDec(int(d)) : details::digitToCharAlpha(int(d-10), bUpper);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// inline constexpr std::size_t constCharLen(const char* str)
// {
//     return (*str == '\0') ? 0 : 1 + constCharLen(str + 1);
// }

//----------------------------------------------------------------------------
template <std::size_t N>
constexpr std::ptrdiff_t strLen(const char (&str)[N])
{
    return std::ptrdiff_t(N - 1); // -1 для исключения нулевого терминатора
}

inline std::ptrdiff_t strLen(const char* str)
{
    std::ptrdiff_t i = 0;
    for(; *str; ++i, ++str) {}
    return i;
}


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace bigint_utils
} // namespace marty

// marty::bigint_utils::
// #include "marty_bigint/marty_bigint.h"
