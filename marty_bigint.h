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

#include <algorithm>
#include <string>
#include <vector>
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


public: // basic ctors & operators

    BigInt() {}
    BigInt(const BigInt &) = default;
    BigInt& operator=(const BigInt &) = default;
    BigInt(BigInt &&) = default;
    BigInt& operator=(BigInt &&) = default;


protected: // from int type construction helpers


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

       if constexpr (sizeof(unsigned_t)>=sizeof(T))
       {
           m_module.emplace_back(unsigned_t(t));
           return;
       }
       else
       {
           T divisor = T(unsigned_t(-1));
           divisor += 1;

           while(t)
           {
               m_module.emplace_back(unsigned_t(t%divisor));
               t /= divisor;
           }
       }

    }
    
    template < typename T, std::enable_if_t< std::is_integral_v<T> && std::is_signed_v<T>, int> = 0 >
    void assign(T t) { assignSigned(t); }

    template < typename T, std::enable_if_t< std::is_integral_v<T> && ! std::is_signed_v<T>, int> = 0 >
    void assign(T t) { assignUnsigned(t); }


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

    static int moduleCompare(const number_holder_t &m1, number_holder_t m2);

    static number_holder_t moduleAdd(const number_holder_t &m1, const number_holder_t &m2);

    // m1 - уменьшаемое
    // m2 - вычитаемое
    // уменьшаемое должно быть больше или равно вычитаемому
    static number_holder_t moduleSub(const number_holder_t& m1, number_holder_t m2);


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
            m.emplace_back(tmp);

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
    
            res.emplace_back(static_cast<unsigned_t>(op(v1, v2)));
        
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


protected: // ctors

    BigInt(int signOther, const number_holder_t &moduleOther)
    : m_module(moduleOther)
    , m_sign(signOther)
    {
        shrinkLeadingZeros();
    }



public: // to integer convertion


public: // to string convertion

    std::string toString() const
    {
        // Временная реализация, используем std::uint64_t и реально длинные числа форматировать не можем

        constexpr const std::size_t unsigned_t_bits = sizeof(unsigned_t)*CHAR_BIT;

        std::uint64_t val = 0;
        
        for(std::size_t i=0; i!=m_module.size(); ++i)
        {
            auto nShift = unsigned_t_bits*i;
            if (nShift>=64)
                break;

            std::uint64_t tmp = std::uint64_t(m_module[i]);
            tmp <<= nShift;
            val |= tmp;
        }

/*
#if defined(__GNUC__) && (__GNUC__ < 11)
        std::ostringstream oss;
        //oss << std::hex;
        oss << val;
        return oss.str();
#else
        char buf[64];
        auto result = std::to_chars(&buf[0], &buf[64], val, fmt, precision);
*/

        return std::string(m_sign<0 ? 1u : 0u, '-') + std::to_string(val);

    }

    std::string to_string() const { return toString(); }

    // operator std::string() const
    // {
    //     return toString();
    // }


public: // compare, ==, !=, <, <=, >, >=

    bool operator==(const BigInt &b) const  { return compareImpl(b)==0; }
    bool operator!=(const BigInt &b) const  { return compareImpl(b)!=0; }
    bool operator<=(const BigInt &b) const  { return compareImpl(b)<=0; }
    bool operator< (const BigInt &b) const  { return compareImpl(b)< 0; }
    bool operator>=(const BigInt &b) const  { return compareImpl(b)>=0; }
    bool operator> (const BigInt &b) const  { return compareImpl(b)> 0; }


public: // arithmetic operators '+', '-', '/', '*'

    BigInt operator+()                  { return *this; }
    BigInt operator-()                  { return negated(); }

    BigInt operator+(const BigInt &b)   { BigInt res = *this; return res.addImpl(b); }
    BigInt operator-(const BigInt &b)   { BigInt res = *this; return res.subImpl(b); }

    BigInt& operator+=(const BigInt &b) { return addImpl(b); }
    BigInt& operator-=(const BigInt &b) { return subImpl(b); }


public: // logical operators

    explicit operator bool() const { return boolCast(); }
    bool operator !() const        { return !boolCast(); }


public: // shifts

    BigInt  operator<< (int v) const { auto tmp = *this; tmp.shiftLeftImpl(v); return tmp; }
    BigInt& operator<<=(int v)       { shiftLeftImpl(v); return *this; }
    BigInt  operator>> (int v) const { auto tmp = *this; tmp.shiftRightImpl(v); return tmp; }
    BigInt& operator>>=(int v)       { shiftRightImpl(v); return *this; }

public: // bit ops

    BigInt  operator& (const BigInt &b) const { auto tmp = *this; tmp.andImpl(b); return tmp; }
    BigInt  operator| (const BigInt &b) const { auto tmp = *this; tmp.orImpl (b); return tmp; }
    BigInt  operator^ (const BigInt &b) const { auto tmp = *this; tmp.xorImpl(b); return tmp; }

    BigInt& operator&=(const BigInt &b)       { andImpl(b); return *this; }
    BigInt& operator|=(const BigInt &b)       { orImpl (b); return *this; }
    BigInt& operator^=(const BigInt &b)       { xorImpl(b); return *this; }

    BigInt  operator~ () const                { auto tmp = *this; tmp.invImpl(); return tmp; }


}; // class BigInt

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
void BigInt::moduleInc(number_holder_t &m)
{
    for(auto &v: m)
    {
        ++v;
        if (v!=0)
            break;
    }
}

//----------------------------------------------------------------------------
inline
void BigInt::moduleDec(number_holder_t &m)
{
    for(auto &v: m)
    {
        if (v!=0)
        {
            --v;
            break;
        }
        
        --v;
    }
}

//----------------------------------------------------------------------------
inline
int BigInt::moduleCompare(const number_holder_t &m1, number_holder_t m2)
{
    std::size_t maxSize = std::max(m1.size(), m2.size());
    for (std::size_t i = maxSize; i-->0;)
    {
        unsigned_t v1 = 0;
        unsigned_t v2 = 0;

        if (i<m1.size())
            v1 = m1[i];
    
        if (i<m2.size())
            v2 = m2[i];

        if (v1==v2)
            continue;

        if (v1<v2)
            return -1;

        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleAdd(const number_holder_t &m1, const number_holder_t &m2)
{
    std::size_t maxSize = std::max(m1.size(), m2.size());

    number_holder_t res; res.reserve(maxSize);
    bool prevOverflow = false;

    for(std::size_t i=0; i!=maxSize; ++i)
    {
        unsigned_t v1 = 0;
        unsigned_t v2 = 0;

        if (i<m1.size())
            v1 = m1[i];
    
        if (i<m2.size())
            v2 = m2[i];

        unsigned_t r = unsigned_t(v1 + v2);

        bool nextOverflow = (r<v1) || r<v2;

        if (prevOverflow)
        {
            ++r;
            if (r==0) // Ноль меньше единицы, произошло переполнение
                nextOverflow = true;
        }

        prevOverflow = nextOverflow;

        res.emplace_back(r);
    
    }

    if (prevOverflow)
        res.emplace_back(unsigned_t(1));

    return res;
}

//----------------------------------------------------------------------------
// m1 - уменьшаемое
// m2 - вычитаемое
// уменьшаемое должно быть больше или равно вычитаемому
inline
BigInt::number_holder_t BigInt::moduleSub(const number_holder_t &m1, number_holder_t m2)
{
    std::size_t maxSize = std::max(m1.size(), m2.size());

    number_holder_t res; res.reserve(maxSize);
    bool prevOverflow = false;

    for(std::size_t i=0; i!=maxSize; ++i)
    {
        unsigned_t v1 = 0;
        unsigned_t v2 = 0;

        if (i<m1.size())
            v1 = m1[i];
    
        if (i<m2.size())
            v2 = m2[i];

        bool nextOverflow = v1 < v2;

        unsigned_t r = unsigned_t(v1 - v2);

        if (prevOverflow)
        {
            if (r==0)
                nextOverflow = true;
            --r;
        }

        prevOverflow = nextOverflow;

        res.emplace_back(r);
    
    }

    // prevOverflow никогда не должен быть true - выкинуть исключение/ассерт?
    // if (prevOverflow)
    //     res.emplace_back(unsigned_t(1));

    return res;


    // // инвертировать и прибавить 1
    // for(auto &v: m2)
    // {
    //     v = unsigned_t(~v);
    // }
    //  
    // while(m2.size()<m1.size())
    //     m2.emplace_back(unsigned_t(-1));
    //  
    // moduleInc(m2);
    //  
    // return shrinkLeadingZerosCopy(moduleAdd(m1, m2));
}

//----------------------------------------------------------------------------
inline
void BigInt::shrinkLeadingZeros(number_holder_t &m)
{
    while(!m.empty())
    {
        if (m.back()!=0u)
            break;
        m.pop_back();
    }
}

//----------------------------------------------------------------------------
inline
int BigInt::compareImpl(int signOther, const number_holder_t &moduleOther) const
{
    if (m_sign==signOther)
        return moduleCompare(m_module, moduleOther);

    if (m_sign<signOther)
        return -1;

    return 1;
}

//----------------------------------------------------------------------------
inline
BigInt& BigInt::addImpl(int signOther, const number_holder_t &moduleOther)
{
    if (signOther==0) 
        return *this; // Ничего прибавлять не требуется

    if (m_sign==0) // У нас ноль - просто присваиваем второе число
    {
        m_sign   = signOther;
        m_module = moduleOther;
        return *this;
    }

    // Оба знака - не нулевые

    if (m_sign==signOther) // знаки - одинаковые, суммируем модули
    {
        m_module = moduleAdd(m_module, moduleOther);
        return *this;
    }

    // Знаки - разные

    int cmpRes = moduleCompare(m_module, moduleOther);

    if (cmpRes==0) // абсолютные значения равны, в результате - ноль
    {
        m_sign = 0;
        m_module.clear();
        return *this;
    }

    number_holder_t resModule;

    if (cmpRes>0) // this greater than other
       resModule = moduleSub(m_module, moduleOther);
    else // this less than other
       resModule = moduleSub(moduleOther, m_module);

    swap(resModule, m_module);

    // На ноль не проверяем - модули не равны, ноль не может получится
    // Осталось разобраться со знаком результата

    if (m_sign>0)
    {
        if (cmpRes>0)
            m_sign =  1;
        else
            m_sign = -1;
    }
    else // m_sign<0
    {
        if (cmpRes>0)
            m_sign = -1;
        else
            m_sign =  1;
    }

    return *this;
}

//----------------------------------------------------------------------------
inline
BigInt& BigInt::subImpl(int signOther, const number_holder_t &moduleOther)
{
    if (signOther!=0)
        signOther = -signOther;

    return addImpl(signOther, moduleOther);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Отрицательная величина сдвига меняет направление сдвига? Или кинуть исключение?
void BigInt::shiftLeftImpl(int v)
{
    if (v<0)
        throw std::invalid_argument("BigInt: negative shift value");

    if (!m_sign)
        return; // сдвиг нуля даст ноль всё равно

    if (m_module.empty())
        return; // сдвигать нечего, тут лучше ассерт

    while(!m_module.empty() && v>=iChunkSizeBits)
    {
        std::rotate(m_module.rbegin(), m_module.rbegin() + 1, m_module.rend());
        if (m_module[0])
        m_module.emplace_back(m_module[0]);
        m_module[0] = 0;
        v -= iChunkSizeBits;
    }

    while(!m_module.empty() && v>=8)
    {
        moduleShiftLeftHelper<8>(m_module);
        v -= 8;
    }

    while(!m_module.empty() && v>=4)
    {
        moduleShiftLeftHelper<4>(m_module);
        v -= 4;
    }

    while(!m_module.empty() && v>=1)
    {
        moduleShiftLeftHelper<1>(m_module);
        v -= 1;
    }

    checkModuleEmpty();

}

//----------------------------------------------------------------------------
void BigInt::shiftRightImpl(int v)
{
    if (v<0)
        throw std::invalid_argument("BigInt: negative shift value");

    if (!m_sign)
        return; // сдвиг нуля даст ноль всё равно

    while(!m_module.empty() && v>=iChunkSizeBits)
    {
        std::rotate(m_module.begin(), m_module.begin() + 1, m_module.end());
        m_module.pop_back();
        v -= iChunkSizeBits;
    }

    while(!m_module.empty() && v>=8)
    {
        moduleShiftRightHelper<8>(m_module);
        v -= 8;
    }

    while(!m_module.empty() && v>=4)
    {
        moduleShiftRightHelper<4>(m_module);
        v -= 4;
    }

    while(!m_module.empty() && v>=1)
    {
        moduleShiftRightHelper<1>(m_module);
        v -= 1;
    }

    checkModuleEmpty();

}


// Алгоритм Фюрера - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%A4%D1%8E%D1%80%D0%B5%D1%80%D0%B0
// Алгоритм Карацубы - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%9A%D0%B0%D1%80%D0%B0%D1%86%D1%83%D0%B1%D1%8B
// Какой метод умножения чисел эффективнее: алгоритм Фюрера или алгоритм Карацубы?


//----------------------------------------------------------------------------
std::string to_string(const BigInt& b)
{
    return b.toString();
}



//----------------------------------------------------------------------------
} // namespace marty



//----------------------------------------------------------------------------
// namespace std
// {
//  
// inline
// std::string to_string(const marty::BigInt &b)
// {
//     return b.toString();
// }
//  
//  
// } // namespace std

//----------------------------------------------------------------------------



// marty::
// #include "marty_bigint/marty_bigint.h"




