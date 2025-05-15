/*!
    \file
    \brief Библиотека marty::bigint
    
 */
#pragma once

#include <string>
#include <vector>
#include <cstdint>

//
#include "undef_min_max.h"


// uint_fast8_t
// uint_fast16_t
// uint_fast32_t
// uint_fast64_t


#ifndef MARTY_BIGINT_USE_VECTOR

    typedef std::basic_string<> raw_bcd_number_t;

#else

    typedef std::vector<decimal_digit_t> raw_bcd_number_t;

#endif


// #include "marty_bigint/marty_bigint.h"
// marty::
namespace marty {


#if (sizeof(int)==4)
    #define INT_IS_4_BYTES
#endif





} // namespace marty

// marty::
// #include "marty_bigint/marty_bigint.h"

