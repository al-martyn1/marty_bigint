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

    // Делит m1 на m2, остаток от деления остаётся в m1
    static number_holder_t moduleSchoolDiv(number_holder_t &m1, number_holder_t m2);
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

    bool operator==(const BigInt &b) const    { return compareImpl(b)==0; }
    bool operator!=(const BigInt &b) const    { return compareImpl(b)!=0; }
    bool operator<=(const BigInt &b) const    { return compareImpl(b)<=0; }
    bool operator< (const BigInt &b) const    { return compareImpl(b)< 0; }
    bool operator>=(const BigInt &b) const    { return compareImpl(b)>=0; }
    bool operator> (const BigInt &b) const    { return compareImpl(b)> 0; }


public: // arithmetic operators '+', '-', '/', '*', ++, --

    BigInt operator+() const                  { return *this; }
    BigInt operator-() const                  { return negated(); }

    BigInt operator+(const BigInt &b) const   { BigInt res = *this; return res.addImpl(b); }
    BigInt operator-(const BigInt &b) const   { BigInt res = *this; return res.subImpl(b); }

    BigInt& operator+=(const BigInt &b)       { return addImpl(b); }
    BigInt& operator-=(const BigInt &b)       { return subImpl(b); }

    BigInt operator*(const BigInt &b) const   { BigInt res = *this; return res.mulImpl(b); }
    BigInt& operator*=(const BigInt &b)       { return mulImpl(b); }

    BigInt operator/(const BigInt &b) const   { BigInt res = *this; return res.divImpl(b); }
    BigInt& operator/=(const BigInt &b)       { return divImpl(b); }

    BigInt operator%(const BigInt &b) const   { BigInt res = *this; return res.remImpl(b); }
    BigInt& operator%=(const BigInt &b)       { return remImpl(b); }


    BigInt& operator++()    { incImpl(); return *this; } // увеличивает, и возвращает уменьшенное
    BigInt& operator--()    { decImpl(); return *this; } // уменьшает, и возвращает уменьшенное
    BigInt& operator++(int) { auto res = *this; incImpl(); return res; } // увеличивает, и возвращает исходное
    BigInt& operator--(int) { auto res = *this; decImpl(); return res; } // уменьшает, и возвращает исходное


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
bool BigInt::moduleIsZero(const number_holder_t &m)
{
    for(auto &&v : m)
    {
        if (v)
            return false;
    }

    return true;
}

//----------------------------------------------------------------------------
inline
int BigInt::moduleCompare(const number_holder_t &m1, number_holder_t m2, std::size_t beginIdxM1, std::size_t endIdxM1)
{
    if (beginIdxM1>=m1.size())
        beginIdxM1 = m1.size();

    if (endIdxM1>=m1.size())
        endIdxM1 = 0;


    std::size_t maxSize = std::max(beginIdxM1, m2.size());

    for (std::size_t i=maxSize; i-->endIdxM1;)
    {
        unsigned_t v1 = 0;
        unsigned_t v2 = 0;

        const std::size_t i2 = i - endIdxM1; // beginIdxM1;

        if (i<m1.size())
            v1 = m1[i];
    
        if (i2<m2.size())
            v2 = m2[i2];

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
void BigInt::moduleExpandTo(number_holder_t &m, std::size_t size, unsigned_t v)
{
    m.resize(size, v);
}

//----------------------------------------------------------------------------
inline
void BigInt::moduleFill(number_holder_t &m, unsigned_t v, std::size_t b, std::size_t e)
{
    if (e>=m.size())
        e = m.size();

    if (b>=m.size())
        b = 0;

    std::fill(m.begin()+std::ptrdiff_t(b), m.begin()+std::ptrdiff_t(e), v);
}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleAdd(const number_holder_t &m1, const number_holder_t &m2)
{
#if 0
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
#endif

    number_holder_t res = m1;
    moduleAddInplace(res, m2);
    shrinkLeadingZeros(res);
    return res;

}

//----------------------------------------------------------------------------
void BigInt::moduleAddInplace(number_holder_t &m1, const number_holder_t &m2, std::size_t b, std::size_t e) // adds m2 to m1
{
    if (e==std::size_t(-1))
        e = std::max(m1.size(), m2.size());

    m1.reserve(m2.size());

    ++e; // увеличиваем конечный индекс, чтобы не нужно было отдельно обрабатывать последнее переполнение

    if (b==std::size_t(-1))
        b = 0;

    bool prevOverflow = false;

    std::size_t i = b;

    for(; i!=e; ++i)
    {
        unsigned_t v1 = 0;
        unsigned_t v2 = 0;

        const std::size_t i2 = i - b;

        if (i<m1.size())
            v1 = m1[i];
    
        if (i2<m2.size())
            v2 = m2[i2];

        unsigned_t r = unsigned_t(v1 + v2);

        bool nextOverflow = (r<v1) || r<v2;

        if (prevOverflow)
        {
            ++r;
            if (r==0) // Ноль меньше единицы, произошло переполнение
                nextOverflow = true;
        }

        prevOverflow = nextOverflow;

        if (i<m1.size())
        {
            m1[i] = r;
        }
        else
        {
            if (i!=(e-1) || r!=0)
                m1.push_back(r);
        }
    }

    // if (prevOverflow)
    // {
    //     m1.emplace_back(unsigned_t(1));
    //  
    // }

}

//----------------------------------------------------------------------------
// m1 - уменьшаемое
// m2 - вычитаемое
// уменьшаемое должно быть больше или равно вычитаемому
void BigInt::moduleSubInplace(number_holder_t& m1, number_holder_t m2, std::size_t beginIdxM1, std::size_t endIdxM1)
{
    if (beginIdxM1>=m1.size())
        beginIdxM1 = 0;

    if (endIdxM1>=m1.size())
        endIdxM1 = m1.size();


    std::size_t maxSize = std::max(endIdxM1, m2.size());

    // number_holder_t res; res.reserve(maxSize);
    bool prevOverflow = false;

    for(std::size_t i=beginIdxM1; i!=maxSize; ++i)
    {
        unsigned_t v1 = 0;
        unsigned_t v2 = 0;

        const std::size_t i2 = i - beginIdxM1;

        if (i<m1.size())
            v1 = m1[i];
    
        if (i2<m2.size())
            v2 = m2[i2];

        bool nextOverflow = v1 < v2;

        unsigned_t r = unsigned_t(v1 - v2);

        if (prevOverflow)
        {
            if (r==0)
                nextOverflow = true;
            --r;
        }

        prevOverflow = nextOverflow;

        if (i<m1.size())
            m1[i] = r;
    
    }
}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleSub(const number_holder_t &m1, number_holder_t m2)
{
    number_holder_t res = m1;
    moduleSubInplace(res, m2);
    return res;

#if 0
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

        res.push_back(r);
    
    }

    // prevOverflow никогда не должен быть true - выкинуть исключение/ассерт?
    // if (prevOverflow)
    //     res.emplace_back(unsigned_t(1));

    return res;
#endif
}

//----------------------------------------------------------------------------
BigInt& BigInt::incImpl()
{
    if (!m_sign)
    {
        *this = BigInt(std::int8_t(1));
        return *this;
    }

    if (m_sign>0) // Если число больше нуля, и операция инкремента, то вызываем инкремент модуля
    {
        moduleInc(m_module);
        return *this;
    }

    // Число отрицательное, для инкремента надо уменьшить модуль

    moduleDec(m_module);
    shrinkLeadingZeros();
    return *this;
}

//----------------------------------------------------------------------------
BigInt& BigInt::decImpl()
{
    if (!m_sign)
    {
        *this = BigInt(std::int8_t(-1));
        return *this;
    }

    if (m_sign<0) // Если число меньше нуля, и операция декремента, то вызываем инкремент модуля
    {
        moduleInc(m_module);
        return *this;
    }

    // Число положительное, для декремента надо уменьшить модуль

    moduleDec(m_module);
    shrinkLeadingZeros();
    return *this;
}

    // static void moduleInc(number_holder_t &m);
    // static void moduleDec(number_holder_t &m);

    // void checkModuleEmpty()    { if (m_module.empty()) m_sign = 0; }
    //  
    // static void shrinkLeadingZeros(number_holder_t &m);
    // static number_holder_t shrinkLeadingZerosCopy(number_holder_t m) { shrinkLeadingZeros(m); return m; }
    // void shrinkLeadingZeros()  { shrinkLeadingZeros(m_module); checkModuleEmpty(); }



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
inline
void BigInt::moduleShiftLeft(number_holder_t &m, int v)
{
    if (m.empty())
        return;

    const int nBitsFullChunks = v - v%iChunkSizeBits;
    const int nFullChunks     = nBitsFullChunks / iChunkSizeBits;
    v -= nBitsFullChunks;

    // Сдвиг влево - в сторону старших разрядов
    // Младшие разряды у нас идут сначала
    // Значит, надо просто вставить nFullChunks нулей в начало

    m.insert(m.begin(), std::size_t(nFullChunks), 0);

    // while(!m.empty() && v>=iChunkSizeBits)
    // {
    //     std::rotate(m.rbegin(), m.rbegin() + 1, m.rend());
    //     if (m[0])
    //         m.emplace_back(m[0]);
    //     m[0] = 0;
    //     v -= iChunkSizeBits;
    // }

    while(!m.empty() && v>=8)
    {
        moduleShiftLeftHelper<8>(m);
        v -= 8;
    }

    while(!m.empty() && v>=4)
    {
        moduleShiftLeftHelper<4>(m);
        v -= 4;
    }

    while(!m.empty() && v>=1)
    {
        moduleShiftLeftHelper<1>(m);
        v -= 1;
    }
}

//----------------------------------------------------------------------------
inline
void BigInt::moduleShiftRight(number_holder_t &m, int v)
{
    const int nBitsFullChunks = v - v%iChunkSizeBits;
    const int nFullChunks     = nBitsFullChunks / iChunkSizeBits;
    v -= nBitsFullChunks;

    // Сдвиг вправо - в сторону младших разрядов
    // Младшие разряды у нас идут сначала
    // выдвигаемые значения просто пропадают
    // Значит, надо просто удалить nFullChunks элементов в начале

    if (m.size()<=std::size_t(nFullChunks))
    {
        m.clear();
        return;
    }

    m.erase(m.begin(), m.begin()+ptrdiff_t(nFullChunks));


    // while(!m.empty() && v>=iChunkSizeBits)
    // {
    //     std::rotate(m.begin(), m.begin() + 1, m.end());
    //     m.pop_back();
    //     v -= iChunkSizeBits;
    // }

    while(!m.empty() && v>=8)
    {
        moduleShiftRightHelper<8>(m);
        v -= 8;
    }

    while(!m.empty() && v>=4)
    {
        moduleShiftRightHelper<4>(m);
        v -= 4;
    }

    while(!m.empty() && v>=1)
    {
        moduleShiftRightHelper<1>(m);
        v -= 1;
    }
}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleShiftLeftCopy(const number_holder_t &m, int v)
{
    number_holder_t res = m;
    moduleShiftLeft(res, v);
    return res;
}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleShiftRightCopy(const number_holder_t &m, int v)
{
    number_holder_t res = m;
    moduleShiftRight(res, v);
    return res;
}

//----------------------------------------------------------------------------
inline
void BigInt::shiftLeftImpl(int v)
{
    if (v<0)
        throw std::invalid_argument("BigInt: negative shift value");

    if (!m_sign)
        return; // сдвиг нуля даст ноль всё равно

    moduleShiftLeft(m_module, v);
    checkModuleEmpty();
}

//----------------------------------------------------------------------------
inline
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

//----------------------------------------------------------------------------

// Алгоритм Фюрера - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%A4%D1%8E%D1%80%D0%B5%D1%80%D0%B0
// Алгоритм Карацубы - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%9A%D0%B0%D1%80%D0%B0%D1%86%D1%83%D0%B1%D1%8B
// Какой метод умножения чисел эффективнее: алгоритм Фюрера или алгоритм Карацубы?

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleSchoolMul(const number_holder_t &a, const number_holder_t &b)
{
/*
           1234
         *
            864
---------------
             16  4*4
            120  4*3
            800  4*2
           4000  4*1
---------------
            240  6*4
           1800  6*3
          12000  6*2
          60000  6*1
---------------
           3200  8*4
          24000  8*3
         160000  8*2
         800000  8*1
---------------
16+120+800+4000+240+1800+12000+60000+3200+24000+160000+800000=1066176

Стартовый разряд равен сумме индексов умножаемых разрядов.

*/
    // Желательно внутренний цикл сделать более длинным, чем внешний
    const number_holder_t &m1 =  (a.size()<b.size()) ? a : b;
    const number_holder_t &m2 = !(a.size()<b.size()) ? a : b;

    number_holder_t res; res.resize(m1.size()+m2.size()+1);
    //number_holder_t tmp; tmp.resize(m1.size()+m2.size()+1);
    number_holder_t tmp; tmp.resize(2);

    moduleFill(res, 0u);
    moduleFill(tmp, 0u);

    for(std::size_t i1=0; i1!=m1.size(); ++i1)
    {
        for(std::size_t i2=0; i2!=m2.size(); ++i2)
        {
            const unsigned2_t m1_i1 = unsigned2_t(m1[i1]);
            const unsigned2_t m2_i2 = unsigned2_t(m2[i2]);
            unsigned2_t tmpMul = unsigned2_t(m1_i1*m2_i2);
            const std::size_t idx = i1+i2;
            tmp[0] += unsigned_t( tmpMul );
            tmp[1] += unsigned_t((tmpMul>>(sizeof(unsigned_t)*CHAR_BIT)));
            moduleAddInplace(res, tmp, idx);
            moduleFill(tmp, 0u);
        }
    }

    shrinkLeadingZeros(res);
    return res;
}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleFurerMul(const number_holder_t &a, const number_holder_t &b)
{
    // Алгоритм Фюрера - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%A4%D1%8E%D1%80%D0%B5%D1%80%D0%B0
    // Интересно про свёртки

    // Проверил на бамажке - вроде работает. А что самое прикольное - гораздо проще всратого умножения столбиком.
    // В статье по ссылке - число маленькое, было интересно, когда элемент свертки перевалит за два разряда.
    // Будет не лень - настучу свой пример.

    const number_holder_t &m1 =  (a.size()<b.size()) ? a : b;
    const number_holder_t &m2 = !(a.size()<b.size()) ? a : b;

    std::vector<number_holder_t> convolution = std::vector<number_holder_t>(m1.size()+m2.size()+1);
    for(std::size_t i1=0; i1!=m1.size(); ++i1)
    {
        for(std::size_t i2=0; i2!=m2.size(); ++i2)
        {
            // convolution[i1+i2] += m1[i1] * m2[i2];
            auto mTmp = BigInt( unsigned2_t(unsigned2_t(m1[i1])*unsigned2_t(m2[i2])) ).m_module;
            moduleAddInplace(convolution[i1+i2], mTmp);
        }
    }

    // Выдрано из реализации BCD десятичных чисел с плавающей точкой
    // unsigned overflow = 0;
    //  
    // for( std::size_t i=0; i!=convSize; ++i )
    // {
    //     convolution[i] += overflow;
    //     overflow = bcdCorrectOverflow(convolution[i]);
    //     multRes.push_back((decimal_digit_t)convolution[i]);
    // }
    //  
    // if (overflow)
    // {
    //     multRes.push_back((decimal_digit_t)overflow);
    // }

    // Переделал под BigInt, вроде даже работает


    number_holder_t res; res.reserve(m1.size()+m2.size()+1); // res.resize(m1.size()+m2.size()+1);


    // у нас переполнение не один признак, а число типа unsigned_t

    number_holder_t overflow;

    for(std::size_t i=0; i!=convolution.size(); ++i)
    {
        // Конвертируем одиночный unsigned_t overflow предыдущего переполнения
        // в number_holder_t через использование конструктора BigInt 
        // из интегрального типа, и сразу берем модуль
        // Этот модуль передаём в функцию сложения 
        //moduleAddInplace(convolution[i], BigInt(overflow).m_module); // convolution[i] += overflow;
        moduleAddInplace(convolution[i], overflow); // convolution[i] += overflow;
        // if (convolution[i].size()>2)
        //     throw std::runtime_error("moduleFurerMul: somethin goes wrong (1)");

        // auto tmp = convolution[i];
        // tmp.erase(tmp.begin(), tmp.begin()+ptrdiff_t(1));
        // overflow = tmp;
        overflow = convolution[i];
        if (!overflow.empty()) // Чёт я уже подзабыл, нафига я это сделал, надо было прокоментить
            overflow.erase(overflow.begin(), overflow.begin()+ptrdiff_t(1));

        // if (convolution[i].size()>1)
        //     overflow = convolution[i][1];
        // else
        //     overflow = 0;

        // if (convolution[i].empty())
        //     throw std::runtime_error("moduleFurerMul: somethin goes wrong (2)");

        if (!convolution[i].empty())
            res.push_back(convolution[i][0]);
        
    }

    // if (overflow!=0)
    //     res.emplace_back(overflow);
    res.insert(res.end(), overflow.begin(), overflow.end());

    // using unsigned_t      = marty::bigint_details::unsigned_t;
    // using unsigned2_t     = marty::bigint_details::unsigned2_t;

    shrinkLeadingZeros(res);

    return res;

}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleKaratsubaMul(const number_holder_t &a, const number_holder_t &b)
{
    // std::size_t size = m1.size()+m2.size();
    std::size_t size = std::min(a.size(),b.size());
    // if ((a.size()+b.size()) <= 4)
    if (size<=2)
       return moduleSchoolMul(a, b);

    auto mid  = std::max(a.size(), b.size())/2u;
    auto midA = ptrdiff_t(std::min(mid, a.size()));
    auto midB = ptrdiff_t(std::min(mid, b.size()));
    
    auto low1  = number_holder_t(a.begin()       , a.begin() + midA);
    auto high1 = number_holder_t(a.begin() + midA, a.end()         );
    auto low2  = number_holder_t(b.begin()       , b.begin() + midB);
    auto high2 = number_holder_t(b.begin() + midB, b.end()         );
    
    // Конвертим в BigInt, так как у нас есть вычитания, и, хотя в итоге должно получится положительное
    // число, в промежуточных вычислениях теоретически могут получаться отрицательные значения
    // (но я не проверял), а значит, нужна полновесная арифметика с учетом знака.
    // Оверхед на учёт знака минимален.

    auto z0 = BigInt(1, moduleKaratsubaMul(low1, low2));
    auto z1 = BigInt(1, moduleKaratsubaMul(moduleAdd(low1, high1), moduleAdd(low2, high2)));
    auto z2 = BigInt(1, moduleKaratsubaMul(high1, high2));

    auto z2_shifted_by_2mid = BigInt(1, moduleShiftLeftCopy(z2.m_module, 2*int(mid))); // (z2 << (2*std::size_t(mid)))

    return (z2_shifted_by_2mid + (((z1 - z2 - z0) << int(mid))) + z0).m_module;

}

//----------------------------------------------------------------------------
inline
BigInt::number_holder_t BigInt::moduleAutoMul(const number_holder_t &m1, const number_holder_t &m2)
{
    // std::size_t size = m1.size()+m2.size();
    std::size_t size = std::min(m1.size(),m2.size());
    //if (size<12)
    if (size<=6)
        return moduleSchoolMul(m1, m2);
    // //else if (size<100)
    // else if (size<50)
    //     return moduleFurerMul(m1, m2);
    // else
    //     return moduleKaratsubaMul(m1, m2);
    return moduleFurerMul(m1, m2);
}

//----------------------------------------------------------------------------
inline
BigInt& BigInt::mulImpl(const BigInt &b)
{
    m_sign = m_sign*b.m_sign;
    if (m_sign==0)
    {
        m_module.clear();
        return *this;
    }

    switch(s_multiplicationMethod)
    {
        case MultiplicationMethod::school:
             m_module = moduleSchoolMul(m_module, b.m_module);
             break;

        case MultiplicationMethod::karatsuba:
             m_module = moduleKaratsubaMul(m_module, b.m_module);
             break;

        case MultiplicationMethod::furer:
             m_module = moduleFurerMul(m_module, b.m_module);
             break;

        case MultiplicationMethod::auto_: [[fallthrough]];
        default:
             {
                 m_module = moduleAutoMul(m_module, b.m_module);
             }
    }

    return *this;
}

//----------------------------------------------------------------------------
inline
const char* BigInt::getMultiplicationMethodName(MultiplicationMethod mm)
{
    switch(mm)
    {
        case MultiplicationMethod::school:
             return "school";

        case MultiplicationMethod::karatsuba:
             return "karatsuba";

        case MultiplicationMethod::furer:
             return "furer";

        case MultiplicationMethod::auto_: [[fallthrough]];
        default:
             return "auto";
    }

}

//----------------------------------------------------------------------------
inline
const char* BigInt::getMultiplicationMethodName()
{
    return getMultiplicationMethodName(s_multiplicationMethod);
}

//----------------------------------------------------------------------------
// Делит m1 на m2, остаток от деления остаётся в m1
// static number_holder_t moduleSchoolDiv(number_holder_t &m1, const number_holder_t &m2);
// static bool moduleIsZero(const number_holder_t &m);
// static int moduleCompare(const number_holder_t &m1, number_holder_t m2, std::size_t beginIdxM1=std::size_t(-1), std::size_t endIdxM1=std::size_t(-1));
inline
BigInt::number_holder_t BigInt::moduleSchoolDiv(number_holder_t &m1, number_holder_t m2)
{
    shrinkLeadingZeros(m1);
    if (m1.empty())
        return m1;

    shrinkLeadingZeros(m2);

    if (moduleIsZero(m2))
        throw std::overflow_error("BigInt: division by zero");

    // Проверяем, что m1 < m2. Если да, результат 0, остаток m1.
    if (moduleCompare(m1, m2) < 0)
    {
        return number_holder_t(); // Возвращаем 0
    }

    std::size_t nShift = m1.size() - m2.size() + 1u;

    // reverse result
    number_holder_t rRes; rRes.reserve(nShift);

    while(nShift-->0)
    {
        const std::size_t idxM1 = m2.size()+nShift-1u;
        unsigned2_t h1 = unsigned2_t(m1[idxM1]);
        if ((idxM1+1)<m1.size())
        {
            unsigned_t h1_2 = m1[idxM1+1];
            h1 += unsigned2_t(unsigned2_t(h1_2)<<((sizeof(unsigned2_t)/2)*CHAR_BIT));
        }
        const unsigned2_t h2 = unsigned2_t(m2.back());
        unsigned_t qHat = unsigned_t(h1/h2); // back - m2[m2.size()-1] - старшая часть
        if (qHat)
        {
            number_holder_t sub = moduleAutoMul(m2, number_holder_t(1, qHat));
            std::size_t nShiftEnd = nShift+m2.size()+1;
            int cmpRes = moduleCompare(m1, sub, nShiftEnd, nShift);
            while(qHat && cmpRes<0)
            {
                --qHat;
                moduleSubInplace(sub, m2);
                cmpRes = moduleCompare(m1, sub, nShiftEnd, nShift);
            }

            rRes.push_back(qHat);

            moduleSubInplace(m1, sub, nShift, nShiftEnd);
        }
        else
        {
            rRes.push_back(0);
        }

    }

    std::reverse(rRes.begin(), rRes.end());
    shrinkLeadingZeros(rRes);

    return rRes;

}
//----------------------------------------------------------------------------
inline
BigInt& BigInt::divImpl(const BigInt &b) // Делит текущий объект на b
{
    if (b.m_sign==0)
        throw std::overflow_error("BigInt: division by zero");

    m_sign = m_sign*b.m_sign;
    if (m_sign==0)
    {
        m_module.clear();
        return *this;
    }

    m_module = moduleSchoolDiv(m_module, b.m_module);
    shrinkLeadingZeros();

    return *this;
}
//----------------------------------------------------------------------------
inline
BigInt& BigInt::remImpl(const BigInt &b)
{
    if (b.m_sign==0)
        throw std::overflow_error("BigInt: division by zero");

    // m_sign = m_sign*b.m_sign;
    if (m_sign==0)
    {
        m_module.clear();
        return *this;
    }

    // m_sign = 1; // Для остатка - всегда + (или нет?)
    moduleSchoolDiv(m_module, b.m_module);
    shrinkLeadingZeros();

    return *this;
}

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




