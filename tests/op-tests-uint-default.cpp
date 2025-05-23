/*! \file
    \brief Тестим операторы marty::BigInt с дефолтным для текущей системы размером чанка (обычно std::uint32_t)
 */

#ifdef MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE
    #undef MARTY_BIGINT_FORCE_NUMBER_UNDERLYING_TYPE
#endif

#include "op-tests-impl.cpp"

