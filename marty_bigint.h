/*!
    \file
    \brief Библиотека marty::bigint
    
 */
#pragma once

#include "types.h"
#include "utils.h"

#if defined(__GNUC__) && (__GNUC__ < 11)

    #include <sstream>
    #include <stdexcept>
    #include <ios>

#else 

    #include <charconv>

#endif

#if (__cplusplus>=202002L)
    #include <compare>
#endif


#if defined(USE_MARTY_DECIMAL) && USE_MARTY_DECIMAL!=0
    #include "marty_decimal/marty_decimal.h"
#endif


#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <climits>

//
#include "undef_min_max.h"



// #include "marty_bigint/marty_bigint.h"
// marty::
namespace marty {



class BigInt
{

public: // types

    enum MultiplicationMethod
    {
        auto_ = 0,
        school,
        karatsuba,
        furer
    };

    using chunk_type      = marty::bigint_details::unsigned_t;

protected: // member fields

    using unsigned_t      = marty::bigint_details::unsigned_t;
    using unsigned2_t     = marty::bigint_details::unsigned2_t;
    using number_holder_t = marty::bigint_details::number_holder_t;

    constexpr const static inline std::size_t chunkSize  = sizeof(unsigned_t);
    constexpr const static inline int         iChunkSize = int(chunkSize);

    constexpr const static inline std::size_t chunkSizeBits  = CHAR_BIT * chunkSize ;
    constexpr const static inline int         iChunkSizeBits = CHAR_BIT * iChunkSize;

    number_holder_t       m_module;
    int                   m_sign = 0;

    static inline MultiplicationMethod s_multiplicationMethod = MultiplicationMethod::auto_;
    // static inline MultiplicationMethod s_multiplicationMethod = MultiplicationMethod::school;
    // static inline MultiplicationMethod s_multiplicationMethod = MultiplicationMethod::karatsuba;
    // static inline MultiplicationMethod s_multiplicationMethod = MultiplicationMethod::furer;


public: // static methods

    static MultiplicationMethod setMultiplicationMethod(MultiplicationMethod mm)
    {
        std::swap(mm, s_multiplicationMethod);
        return mm;
    }

    static MultiplicationMethod getMultiplicationMethod()
    {
        return s_multiplicationMethod;
    }

    static const char* getMultiplicationMethodName();
    static const char* getMultiplicationMethodName(MultiplicationMethod);


public: // basic ctors & operators

    BigInt() {}
    BigInt(const BigInt &) = default;
    BigInt& operator=(const BigInt &) = default;
    BigInt(BigInt &&) = default;
    BigInt& operator=(BigInt &&) = default;


protected: // from int type construction helpers

    template < typename T, std::enable_if_t< std::is_integral_v<T> && ! std::is_signed_v<T>, int> = 0 >
    static
    number_holder_t moduleFromUnsigned(T t)
    {
       number_holder_t module;

       if constexpr (sizeof(unsigned_t)>=sizeof(T))
       {
           module.push_back(unsigned_t(t));
       }
       else
       {
           T divisor = T(unsigned_t(-1));
           divisor += 1;

           while(t)
           {
               module.push_back(unsigned_t(t%divisor));
               t /= divisor;
           }

           shrinkLeadingZeros(module);
       }

       return module;
    }

    
    template < typename T, std::enable_if_t< std::is_integral_v<T> && std::is_signed_v<T>, int> = 0 >
    void assignSigned(T t)
    {
         assignUnsigned(marty::bigint_utils::toUnsignedAbs(t));
         if (t<0)
             m_sign = -1; // Для отрицательных значений перетираем знак
    }

    template < typename T, std::enable_if_t< std::is_integral_v<T> && ! std::is_signed_v<T>, int> = 0 >
    void assignUnsigned(T t)
    {
       if (t==0)
       {
           m_module.clear();
           m_sign = 0;
           return;
       }

       m_sign = 1;

       m_module = moduleFromUnsigned(t);

       #if 0
       if constexpr (sizeof(unsigned_t)>=sizeof(T))
       {
           m_module.push_back(unsigned_t(t));
       }
       else
       {
           T divisor = T(unsigned_t(-1));
           divisor += 1;

           while(t)
           {
               m_module.push_back(unsigned_t(t%divisor));
               t /= divisor;
           }

           shrinkLeadingZeros();
       }
       #endif
    }
    
    template < typename T, std::enable_if_t< std::is_integral_v<T> && std::is_signed_v<T>, int> = 0 >
    void assign(T t) { assignSigned(t); }

    template < typename T, std::enable_if_t< std::is_integral_v<T> && ! std::is_signed_v<T>, int> = 0 >
    void assign(T t) { assignUnsigned(t); }


    // При разборе не поддерживаем автоматическую 8ричную базу, но её можно явно задать
    template<typename CharIterType>
    static
    CharIterType fromCharsTo(CharIterType b, CharIterType e, BigInt &be, int base=0, bool ignoreGroupSeps=true, bool *pNumberParsed=0)
    {
        if (base!=0 && base!=2 && base!=8 && base!=10 && base!=16)
            throw std::invalid_argument("BigInt::fromCharsTo: invalid base taken");

        if (pNumberParsed)
            *pNumberParsed = false;

        // bool hasLeadingZero = false;

        be.reset();

        // Пропускаем пробелы
        for(; b!=e; ++b)
        {
            if (!bigint_utils::isSpace(*b))
                break;
        }

        if (b==e)
            return b;

        // Проверяем знак
        if (bigint_utils::isSign(*b))
        {
            be.m_sign = bigint_utils::toSign(*b);
            ++b;
        }

        if (b==e)
            return b;

        // Пропускаем пробелы
        for(; b!=e; ++b)
        {
            if (!bigint_utils::isSpace(*b))
                break;
        }

        if (b==e)
            return b;


        int d = 0;

        std::size_t zeroCount = 0;
        // Пропускаем ведущие нули
        for(; b!=e; ++b)
        {
            if (bigint_utils::isGroupSep(*b) && ignoreGroupSeps) // Игнорим разделители в виде апострофов и подчеркиваний - другие не поддерживаем
                continue;

            d = bigint_utils::toDigit(*b);
            if (d!=0)
                break;

            ++zeroCount;
        }


        if (b==e)
        {
            be.reset();
            if (zeroCount>0)
            {
                // У нас - ноль
                if (pNumberParsed)
                    *pNumberParsed = true;
            }
            return b;
        }

        if (zeroCount==1) // Строго один ведущий ноль - возможно, что это префикс
        {
            // Тут проверяем префикс 0b/0x

            if (bigint_utils::isBase(*b))
            {
                if (base!=0) // Наткнулись на символ базы, но у нас есть явно заданная база, поэтому останавливаемся
                {
                    // У нас - ноль
                    be.reset();
                    if (pNumberParsed)
                        *pNumberParsed = true;
                    return b;
                }

                base = bigint_utils::toBase(*b);
                // hasBase = true;
                CharIterType baseIter = b;

                ++b;
                auto tmpDigit = b==e ? -1 : bigint_utils::toDigit(*b);
                if (tmpDigit<0 || tmpDigit>=base) 
                {
                    // У нас есть префикс, но мы наткнулись на конец строки или на нецифровой символ,
                    // или на цифровой символ, который преобразовался в символ больше базы
                    // У нас - ноль, и мы возвращаем итератор на символ базы, так как числа после базы нет, значит, символ базы - это ошибка
                    be.reset();
                    if (pNumberParsed)
                        *pNumberParsed = true;
                    return baseIter;
                }

                // пропускаем нули после явной базы
                for(; b!=e; ++b)
                {
                    if (bigint_utils::isGroupSep(*b) && ignoreGroupSeps) // Игнорим разделители в виде апострофов и подчеркиваний - другие не поддерживаем
                        continue;
        
                    d = bigint_utils::toDigit(*b);
                    if (d!=0)
                        break;
                }

            }
        }

        // be.m_module = moduleFromUnsigned(unsigned(d));

        if (base==0)
            base = 10;

        auto moduleBase = moduleFromUnsigned(unsigned(base));

        // Тут мы уже определели базу, если не была задана
        // Осталось только считать число

        // У нас тут b не равен e

        // У нас уже однозначно годно прочитанный ноль
        if (pNumberParsed)
            *pNumberParsed = true;

        d = bigint_utils::toDigit(*b);
        if (d<0 || d>=base)
        {
            // У нас - ноль
            be.reset();
            return b;
        }

        be.m_module = moduleFromUnsigned(unsigned(d));

        ++b;
        
        for(; b!=e; ++b)
        {
            if (bigint_utils::isGroupSep(*b) && ignoreGroupSeps) // Игнорим разделители в виде апострофов и подчеркиваний - другие не поддерживаем
                continue;

            //auto 
            d = bigint_utils::toDigit(*b);
            if (d<0 || d>=base) // не цифра, или цифра не лезет в базу
            {
                be.reset();
                return b;
            }

            // Тут цифру докидываем в число
            be.m_module = moduleMul(be.m_module, moduleBase);
            moduleAddInplace(be.m_module, moduleFromUnsigned(unsigned(d)));
        }

        shrinkLeadingZeros(be.m_module);

        if (be.m_module.empty())
            be.m_sign = 0;

        if (be.m_sign==0 && !be.m_module.empty())
            be.m_sign = 1;

        return b;
    }

    // template<typename CharIterType>
    // static
    // CharIterType fromCharsTo(CharIterType b, CharIterType e, BigInt &be, int base=0, bool *pNumberParsed)

        // bigint_utils::
        // char digitToChar(int d, bool bUpper)
        // int toDigit(char ch)
        // bool isSpace(char ch)
        // bool isSign(char ch)
        // int toSign(char ch)
        // bool isBase(char ch)
        // int toBase(char ch)

    // number_holder_t       m_module;
    // int                   m_sign = 0;


public: // converters from chars
    
// CharIterType fromCharsTo(CharIterType b, CharIterType e, BigInt &be, int base=0, bool ignoreGroupSeps=true, bool *pNumberParsed=0)
    // Для присваивания из строки c проверкой
    template<typename CharIterType>
    CharIterType assignFromChars(CharIterType b, CharIterType e, int base=0, bool ignoreGroupSeps=true, bool *pNumberParsed=0)
    {
        reset();
        return fromCharsTo(b, e, *this, base, ignoreGroupSeps, pNumberParsed);
    }

    // Через данный конструктор мы инициализируем BigInt длинным числом из строки, если не влезаем в интегральные константы
    // Если разбор фейлится, то кидается исключение.
    // template <std::size_t N> BigInt(const char (&pStrNum)[N], int base=0, bool ignoreGroupSeps=true) // Не будет ли тут оверхида с генерацией кучи ctors под каждую длину строки?
    BigInt(const char *pStrNum, int base=0, bool ignoreGroupSeps=true)
    {
        auto b = &pStrNum[0];
        auto e = b + bigint_utils::strLen(pStrNum);
        bool numberParsed = false;
        auto r = assignFromChars(b, e, base, ignoreGroupSeps, &numberParsed);
        if (r!=e || !numberParsed)
            std::invalid_argument("BigInt: invalid init from char*");
    }

    BigInt(const std::string &numStr, int base=0, bool ignoreGroupSeps=true)
    {
        auto b = numStr.begin();
        auto e = numStr.end();
        bool numberParsed = false;
        auto r = assignFromChars(b, e, base, ignoreGroupSeps, &numberParsed);
        if (r!=e || !numberParsed)
            std::invalid_argument("BigInt: invalid init from std::string");
    }


#if defined(USE_MARTY_DECIMAL) && USE_MARTY_DECIMAL!=0

    // Конвертация в/из marty::Decimal через строки - всё равно из двоичного в десятичное надо конвертировать, 
    // а символы дают просто примерно двойной оверхид по памяти, но пофиг, что-то более оптимальное лень делать

    BigInt(const marty::Decimal &d)
    {
        auto str = d.toString();
        auto b   = str.begin();
        auto e   = b + std::ptrdiff_t(str.size());
        bool numberParsed = false;
        auto r = assignFromChars(b, e, 10 /*base, ignoreGroupSeps, &numberParsed*/);
        if (r!=e || !numberParsed || (*r)!='.')
            std::invalid_argument("BigInt: invalid init from marty::Decimal");
    }

    operator marty::Decimal() const
    {
        return marty::Decimal::fromString(toString());
    }

#endif
    

public: // ctor/operator= from integer types

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt(T t) { assign(t); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator=(T t) { assign(t); return *this; }


protected: // operations implementation helpers

    void checkModuleEmpty()    { if (m_module.empty()) m_sign = 0; }

    static void shrinkLeadingZeros(number_holder_t &m);
    static number_holder_t shrinkLeadingZerosCopy(number_holder_t m) { shrinkLeadingZeros(m); return m; }
    void shrinkLeadingZeros()  { shrinkLeadingZeros(m_module); checkModuleEmpty(); }

    void negate() { m_sign = -m_sign; }
    auto negated() const { auto res = *this; res.negate(); return res; }

    bool boolCast() const { return m_sign!=0; }

    static void moduleInc(number_holder_t &m);
    static void moduleDec(number_holder_t &m);

    // в данном случае - реверсивные значения, сравнение идёт со старших разрядов, от хвоста,
    // beginIdxM1 >= endIdxM1
    static int moduleCompare(const number_holder_t &m1, number_holder_t m2, std::size_t beginIdxM1=std::size_t(-1), std::size_t endIdxM1=std::size_t(-1));
    static bool moduleIsZero(const number_holder_t &m);

    static number_holder_t moduleAdd(const number_holder_t &m1, const number_holder_t &m2);
    static void moduleAddInplace(number_holder_t &m1, const number_holder_t &m2, std::size_t b=std::size_t(-1), std::size_t e=std::size_t(-1)); // adds m2 to m1
    static void moduleExpandTo(number_holder_t &m, std::size_t size, unsigned_t v); // aka resize, with v
    static void moduleFill(number_holder_t &m, unsigned_t v, std::size_t b=std::size_t(-1), std::size_t e=std::size_t(-1)); // fill vector with v

    // m1 - уменьшаемое
    // m2 - вычитаемое
    // уменьшаемое должно быть больше или равно вычитаемому
    static number_holder_t moduleSub(const number_holder_t& m1, number_holder_t m2);
    // Вычитаем m2 только из части разрядов модуля m1 - требуется для деления
    static void moduleSubInplace(number_holder_t& m1, number_holder_t m2, std::size_t beginIdxM1=std::size_t(-1), std::size_t endIdxM1=std::size_t(-1));


    // Сдвиг влево увеличивает число
    // Для сдвига влево нам надо вставить нулевой элемент в начало
    // Выдвигаемый бит(биты) надо взять со старших позиций
    // и переместить их на младшие разряды

    template<std::size_t size>
    static void moduleShiftLeftHelper(number_holder_t &m)
    {
        unsigned_t tmp = 0;
        for(auto &v : m)
        {
            unsigned_t newTmp = unsigned_t(bigint_utils::shiftBitsToLow<size, unsigned_t>(v) & bigint_utils::makeLowBitsMask<size, unsigned_t>());
            v <<= size;
            v |= tmp;
            tmp = newTmp;
        }

        if (tmp)
            m.push_back(tmp);

        // shrinkLeadingZeros(); // так как мы не отбрасываем биты, 
        // а расширяем размер числа (если появился сдвиг в новые разряды), 
        // то нет нужды проверять наличие ведущих нулей
    }

    // Сдвиг вправо уменьшает число
    template<std::size_t size>
    static void moduleShiftRightHelper(number_holder_t &m)
    {
        unsigned_t tmp = 0;

        for (std::size_t i=m.size(); i-->0;)
        {
            auto &v = m[i];
            // Сначала применяем маску к младшим разрядам, потом перемещаем биты на старшие позиции
            unsigned_t newTmp = bigint_utils::shiftBitsToHigh<size, unsigned_t>(unsigned_t(v & bigint_utils::makeLowBitsMask<size, unsigned_t>()));
            v >>= size;
            v |= tmp;
            tmp = newTmp;
        }

        // а вот тут мы ничего не делаем, биты выдвигаемые с младших разрядов, просто теряются
        shrinkLeadingZeros(m);

    }

    static void moduleShiftLeft(number_holder_t &m, int v);
    static void moduleShiftRight(number_holder_t &m, int v);
    static number_holder_t moduleShiftLeftCopy(const number_holder_t &m, int v);
    static number_holder_t moduleShiftRightCopy(const number_holder_t &m, int v);

    static number_holder_t moduleFurerMul(const number_holder_t &m1, const number_holder_t &m2);
    static number_holder_t moduleKaratsubaMul(const number_holder_t &m1, const number_holder_t &m2);
    static number_holder_t moduleSchoolMul(const number_holder_t &m1, const number_holder_t &m2);
    static number_holder_t moduleAutoMul(const number_holder_t &m1, const number_holder_t &m2);
    static number_holder_t moduleMul(const number_holder_t &m1, const number_holder_t &m2);

    // Делит m1 на m2, остаток от деления остаётся в m1
    static number_holder_t moduleSchoolDiv(number_holder_t &m1, number_holder_t m2);
    static number_holder_t moduleDiv(number_holder_t &m1, number_holder_t m2) { return moduleSchoolDiv(m1, m2); }
    BigInt& divImpl(const BigInt &b); // Делит текущий объект на b
    BigInt& remImpl(const BigInt &b); // получает остаток от деления в текущем объекте (всегда положительный)
    //static bool moduleIsZero(const number_holder_t &m);


    // Отрицательная величина сдвига меняет направление сдвига? Или кинуть исключение?
    void shiftLeftImpl(int v);
    void shiftRightImpl(int v);


    template<typename Op>
    number_holder_t moduleBitOpImpl(const number_holder_t &m1, const number_holder_t &m2, Op op)
    {
        std::size_t maxSize = std::max(m1.size(), m2.size());
    
        number_holder_t res; res.reserve(maxSize);
    
        for(std::size_t i=0; i!=maxSize; ++i)
        {
            unsigned_t v1 = 0;
            unsigned_t v2 = 0;
    
            if (i<m1.size())
                v1 = m1[i];
        
            if (i<m2.size())
                v2 = m2[i];
    
            res.push_back(static_cast<unsigned_t>(op(v1, v2)));
        
        }

        shrinkLeadingZeros(res);
        return res;
    }

    BigInt& andImpl(const BigInt &other) { m_module = moduleBitOpImpl(m_module, other.m_module   , [](auto i1, auto i2) { return i1&i2; } ); checkModuleEmpty(); return *this; }
    BigInt& orImpl (const BigInt &other) { m_module = moduleBitOpImpl(m_module, other.m_module   , [](auto i1, auto i2) { return i1|i2; } ); checkModuleEmpty(); return *this; }
    BigInt& xorImpl(const BigInt &other) { m_module = moduleBitOpImpl(m_module, other.m_module   , [](auto i1, auto i2) { return i1^i2; } ); checkModuleEmpty(); return *this; }
    BigInt& invImpl()                    { m_module = moduleBitOpImpl(m_module, number_holder_t(), [](auto i1, auto i2) { MARTY_ARG_USED(i2); return ~i1; } ); checkModuleEmpty(); return *this; }


    BigInt& addImpl(int signOther, const number_holder_t &moduleOther);
    BigInt& addImpl(const BigInt &b) { return addImpl(b.m_sign, b.m_module); }

    BigInt& subImpl(int signOther, const number_holder_t &moduleOther);
    BigInt& subImpl(const BigInt &b) { return subImpl(b.m_sign, b.m_module); }

    int compareImpl(int signOther, const number_holder_t &moduleOther) const;
    int compareImpl(const BigInt& b) const { return compareImpl(b.m_sign, b.m_module); }

    BigInt& mulImpl(const BigInt &b);

    BigInt& incImpl();
    BigInt& decImpl();


protected: // ctors

    BigInt(int signOther, const number_holder_t &moduleOther)
    : m_module(moduleOther)
    , m_sign(signOther)
    {
        shrinkLeadingZeros();
    }

    BigInt(int signOther, number_holder_t &&moduleOther)
    : m_module(std::move(moduleOther))
    , m_sign(signOther)
    {
        shrinkLeadingZeros();
    }


protected: // misc methods

    void clear()
    {
        m_sign = 0;
        m_module.clear();
    }

    void reset() { clear(); }

public: // misc methods

    // Возвращает хранимый размер в битах
    std::size_t size() const
    {
        if (m_sign==0)
            return 0u;

        return chunkSizeBits*m_module.size();
    }

    unsigned_t getLowChunk() const
    {
        return m_sign==0 || m_module.empty() ? unsigned_t(0) : m_module[0];
    }

    unsigned_t getHighChunk() const
    {
        return m_sign==0 || m_module.empty() ? unsigned_t(0) : m_module.back();
    }


protected: // to integral type convertion helpers

    bool moduleToIntegralConvertionHelper(std::uint64_t &t) const;
    bool moduleToIntegralConvertionHelper(std::int64_t  &t) const;


public: // to integral convertion


// inline
// bool BigInt::moduleToIntegralConvertionHelper(std::int64_t &t) const
// {
//     t = 0;
//  
//     if (!m_sign)
//         return true; // валидный ноль
//  
//     std::uint64_t t_ = 0;
//     if (!moduleToIntegralConvertionHelper(t_))
//     {
//         // У нас не влезло даже в uint64_t
//         if (m_sign<0)
//             t = ~std::int64_t(t_) + std::int64_t(1);
//         else
//             t = std::int64_t(t_);
//         return false;
//     }
//  
//     constexpr const auto int64_max = std::numeric_limits<std::int64_t>::max();
//     constexpr const auto uint64_int64_max = static_cast<std::uint64_t>(int64_max);
//     constexpr const auto uint64_int64_min_abs = static_cast<std::uint64_t>(int64_max) + 1;
//  
//     if (m_sign<0)
//     {
//         t = - std::int64_t(t_);
//         return t_ <= uint64_int64_min_abs;
//     }
//  
//     t = std::int64_t(t_);
//     return t_ <= uint64_int64_max;
// }


// inline
// bool BigInt::moduleToIntegralConvertionHelper(std::uint64_t &t) const
// {
//     t = 0;
//  
//     if (!m_sign)
//         return true; // валидный ноль
//  
//     auto module = m_module;
//     shrinkLeadingZeros(module);
//     if (module.empty())
//         return true; // валидный ноль
//  
//     const auto maxShiftBitsValue  = int(sizeof(t)*CHAR_BIT);
//  
//     const auto shiftBitsStepValue = int(sizeof(unsigned_t)*CHAR_BIT);
//  
//     for(std::size_t idx=0u; idx!=module.size(); ++idx)
//     {
//         const auto curShift = int(shiftBitsStepValue*idx);
//         if (curShift>=maxShiftBitsValue)
//             return false; // ненулевое значение (а нулевых ведущих у нас нет) сдвигаем за пределы целевого типа - конвертация прошла с усечением.
//  
//         const std::uint64_t mi = std::uint64_t(module[idx]); // очередная часть
//         const std::uint64_t miShifted = std::uint64_t(mi<<curShift);
//         t |= miShifted;
//     }
//  
//     return true; // текущий модуль влез в целевое значение
// }


    template < typename T, std::enable_if_t< std::is_floating_point_v<T>, int> = 0 >
    MARTY_BIGINT_ARITHMETIC_CONVERTION_TYPE
    operator T() const
    {
        T t = T(0.0);

        if (!m_sign)
            return t;

        auto module = m_module;
        shrinkLeadingZeros(module);
        if (module.empty())
            return t;

        const auto shiftBitsStepValue = int(sizeof(unsigned_t)*CHAR_BIT);

        for(std::size_t idx=0u; idx!=module.size(); ++idx)
        {
            const auto curShift = int(shiftBitsStepValue*idx);

            // https://en.cppreference.com/w/cpp/numeric/math/ldexp
            t += std::ldexp(module[idx], curShift);
        }

        return t;
    }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    MARTY_BIGINT_ARITHMETIC_CONVERTION_TYPE
    operator T() const
    {
        if constexpr (std::is_signed_v<T>)
        {
            std::int64_t t = 0;
            moduleToIntegralConvertionHelper(t);
            return T(t);
        }
        else
        {
            std::uint64_t t = 0;
            moduleToIntegralConvertionHelper(t);
            return T(t);
        }
    }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    T checkedConvert(bool *pValid=0) const
    {
        if (pValid)
            *pValid = true;

        if constexpr (std::is_signed_v<T>)
        {
            std::int64_t t = 0;

            if (!moduleToIntegralConvertionHelper(t))
            {
                if (pValid)
                    *pValid = false;
            }
            if (pValid)
            {
                if (t > std::int64_t(std::numeric_limits<T>::max()))
                   *pValid = false;
            }
            return T(t);
        }
        else
        {
            std::uint64_t t = 0;
            if (!moduleToIntegralConvertionHelper(t))
            {
                if (pValid)
                    *pValid = false;
            }
            if (pValid)
            {
                if (t > std::uint64_t(std::numeric_limits<T>::max()))
                   *pValid = false;
                else if (t < std::uint64_t(std::numeric_limits<T>::min()))
                   *pValid = false;
            }
            return T(t);
        }
    
    }


public: // to string convertion

    // Цифры в обратном порядке - старшие в конце
    std::string moduleToStringReversed(int base, bool upperCase=true) const;
    std::string moduleToString(int base, bool upperCase=true) const;

    std::string toString() const;
    std::string to_string() const { return toString(); }

    // Нужно уметь задавать базу, наличие префикса, регистр
    // Префикс всегда в нижнем регистре
    // Группы не делаем, оставляем это для marty::format
    std::string toStringEx(int base, bool upperCase=true, bool addPrefix=true) const;


public: // compare, ==, !=, <, <=, >, >=

#if (__cplusplus>=202002L)

    // https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison
    // https://en.cppreference.com/w/cpp/utility/compare/strong_ordering.html

    std::strong_ordering operator<=>(const BigInt &b) const
    {
         auto cmpRes = compareImpl(b);
         if (!cmpRes)
             return std::strong_ordering::equal;

         return (cmpRes<0) ? std::strong_ordering::less : std::strong_ordering::greater;
    }

#endif

    bool operator==(const BigInt &b) const    { return compareImpl(b)==0; }
    bool operator!=(const BigInt &b) const    { return compareImpl(b)!=0; }
    bool operator<=(const BigInt &b) const    { return compareImpl(b)<=0; }
    bool operator< (const BigInt &b) const    { return compareImpl(b)< 0; }
    bool operator>=(const BigInt &b) const    { return compareImpl(b)>=0; }
    bool operator> (const BigInt &b) const    { return compareImpl(b)> 0; }

#if (__cplusplus>=202002L)

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    std::strong_ordering operator<=>(T t) const { return operator<=>(BigInt(t)); }

#endif

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    bool operator==(T t) const { return operator==(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    bool operator!=(T t) const { return operator!=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    bool operator<=(T t) const { return operator<=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    bool operator< (T t) const { return operator< (BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    bool operator>=(T t) const { return operator>=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    bool operator> (T t) const { return operator> (BigInt(t)); }


public: // arithmetic operators '+', '-', '/', '*', ++, --

    BigInt operator+() const                  { return *this; }
    BigInt operator-() const                  { return negated(); }

    BigInt operator+(const BigInt &b) const   { BigInt res = *this; return res.addImpl(b); }
    BigInt operator-(const BigInt &b) const   { BigInt res = *this; return res.subImpl(b); }

    BigInt& operator+=(const BigInt &b)       { return addImpl(b); }
    BigInt& operator-=(const BigInt &b)       { return subImpl(b); }

    BigInt operator*(const BigInt &b) const   { BigInt res = *this; return res.mulImpl(b); }
    BigInt operator/(const BigInt &b) const   { BigInt res = *this; return res.divImpl(b); }
    BigInt operator%(const BigInt &b) const   { BigInt res = *this; return res.remImpl(b); }

    BigInt& operator*=(const BigInt &b)       { return mulImpl(b); }
    BigInt& operator/=(const BigInt &b)       { return divImpl(b); }
    BigInt& operator%=(const BigInt &b)       { return remImpl(b); }


    BigInt& operator++()    { incImpl(); return *this; } // увеличивает, и возвращает уменьшенное
    BigInt& operator--()    { decImpl(); return *this; } // уменьшает, и возвращает уменьшенное
    BigInt& operator++(int) { auto res = *this; incImpl(); return res; } // увеличивает, и возвращает исходное
    BigInt& operator--(int) { auto res = *this; decImpl(); return res; } // уменьшает, и возвращает исходное


    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator+(T t) const { return operator+(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator-(T t) const { return operator-(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator*(T t) const { return operator*(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator/(T t) const { return operator/(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator%(T t) const { return operator%(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator+=(T t) const { return operator+=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator-=(T t) const { return operator-=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator*=(T t) const { return operator*=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator/=(T t) const { return operator/=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator%=(T t) const { return operator%=(BigInt(t)); }



public: // logical operators

    explicit operator bool() const { return boolCast(); }
    bool operator !() const        { return !boolCast(); }


public: // shifts

    BigInt  operator<< (int v) const { auto tmp = *this; tmp.shiftLeftImpl(v); return tmp; }
    BigInt  operator>> (int v) const { auto tmp = *this; tmp.shiftRightImpl(v); return tmp; }

    BigInt& operator>>=(int v)       { shiftRightImpl(v); return *this; }
    BigInt& operator<<=(int v)       { shiftLeftImpl(v); return *this; }

public: // bit ops

    BigInt  operator& (const BigInt &b) const { auto tmp = *this; tmp.andImpl(b); return tmp; }
    BigInt  operator| (const BigInt &b) const { auto tmp = *this; tmp.orImpl (b); return tmp; }
    BigInt  operator^ (const BigInt &b) const { auto tmp = *this; tmp.xorImpl(b); return tmp; }

    BigInt& operator&=(const BigInt &b)       { andImpl(b); return *this; }
    BigInt& operator|=(const BigInt &b)       { orImpl (b); return *this; }
    BigInt& operator^=(const BigInt &b)       { xorImpl(b); return *this; }

    BigInt  operator~ () const                { auto tmp = *this; tmp.invImpl(); return tmp; }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator&(T t) const { return operator&(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator|(T t) const { return operator|(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt operator^(T t) const { return operator^(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator&=(T t) const { return operator&=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator|=(T t) const { return operator|=(BigInt(t)); }

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    BigInt& operator^=(T t) const { return operator^=(BigInt(t)); }

}; // class BigInt

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#include "impl/marty_bigint.h"

//----------------------------------------------------------------------------
} // namespace marty

// marty::
// #include "marty_bigint/marty_bigint.h"

