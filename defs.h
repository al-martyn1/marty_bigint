/*! \file
    \brief Definitions for marty_format
 */


#pragma once

// #ifndef MARTY_UTF_MARTY_DECIMAL_H
// #define MARTY_UTF_MARTY_DECIMAL_H

// #ifndef MARTY_UTF_ASSERT_FAIL
//  
//     //#if defined(UMBA_ASSERT_FAIL)
//  
//         #define MARTY_UTF_ASSERT_FAIL(msg)     throw std::runtime_error(msg)
//  
//     //#else
//  
//     //    #define MARTY_UTF_ASSERT_FAIL(msg)    UMBA_ASSERT_FAIL(msg)
//  
//     //#endif
//  
// #endif
//  
//----------------------------------------------------------------------------
#ifndef MARTY_ARG_USED

    //! Подавление варнинга о неиспользованном аргументе
    #define MARTY_ARG_USED(x)                   (void)(x)

#endif


#if !defined(USE_MARTY_DECIMAL)
    #define USE_MARTY_DECIMAL 1
#endif

// default arithmetic convertion is implicit
#if !defined(MARTY_BIGINT_USE_EXPLICIT_ARITHMETIC_CONVERTION)
    #define MARTY_BIGINT_USE_EXPLICIT_ARITHMETIC_CONVERTION 0
#endif

#if (MARTY_BIGINT_USE_EXPLICIT_ARITHMETIC_CONVERTION!=0)
    #define MARTY_BIGINT_ARITHMETIC_CONVERTION_TYPE explicit
#else
    #define MARTY_BIGINT_ARITHMETIC_CONVERTION_TYPE
#endif

// Надо настроить MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE в std::uint8_t
// если задан макрос MARTY_BIGINT_USE_MIN_SIZE_CHUNKS != 0
