/*! \file
    \brief Тестим операторы marty::BigInt с чанком std::uint8_t
 */

#ifdef MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE
    #undef MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE
#endif

#ifndef MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE
    #define MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE  std::uint8_t
#endif

#include "op-tests-impl.cpp"

