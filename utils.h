/*!
    \file
    \brief Типы для marty::BigInt
 */
#pragma once

#include "defs.h"

//
#include <type_traits>

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




} // namespace bigint_utils
} // namespace marty

// marty::bigint_utils::
// #include "marty_bigint/marty_bigint.h"
