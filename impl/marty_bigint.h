#pragma once

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
BigInt::number_holder_t BigInt::moduleMul(const number_holder_t &m1, const number_holder_t &m2)
{
    switch(s_multiplicationMethod)
    {
        case MultiplicationMethod::school:
             return moduleSchoolMul(m1, m2);

        case MultiplicationMethod::karatsuba:
             return moduleKaratsubaMul(m1, m2);

        case MultiplicationMethod::furer:
             return moduleFurerMul(m1, m2);

        case MultiplicationMethod::auto_: [[fallthrough]];
        default:
             return moduleAutoMul(m1, m2);
    }
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

    m_module = moduleMul(m_module, b.m_module);

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

    m_module = moduleDiv(m_module, b.m_module);
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
    moduleDiv(m_module, b.m_module);
    shrinkLeadingZeros();

    return *this;
}

//----------------------------------------------------------------------------
inline
std::string BigInt::moduleToStringReversed(int base, bool upperCase) const
{
    if (base!=2 && base!=8 && base!=10 && base!=16)
        throw std::invalid_argument("BigInt::moduleToStringReversed: invalid base taken");

    if (m_sign==0 || m_module.empty())
        return std::string("0");

    std::string resStr;

    if (base==2)
    {
        resStr.reserve(m_module.size()*chunkSizeBits);
        for(auto chunk : m_module)
        {
            for(std::size_t i=0; i!=chunkSizeBits; ++i, chunk>>=1)
            {
                resStr.append(1, bigint_utils::digitToChar(chunk&1, upperCase));
            }
        }
    }
    else if (base==16)
    {
        constexpr const std::size_t chunkSizeHexDigits = chunkSizeBits/4;
        resStr.reserve(m_module.size()*chunkSizeHexDigits);
        for(auto chunk : m_module)
        {
            for(std::size_t i=0; i!=chunkSizeHexDigits; ++i, chunk>>=4)
            {
                resStr.append(1, bigint_utils::digitToChar(chunk&0xF, upperCase));
            }
        }
    }
    else if (base==10)
    {
        number_holder_t module10; module10.reserve(m_module.size());
    
        const int chunkPwr10 = bigint_utils::getTypeDecimalDigits<unsigned_t>();
        const BigInt biDividerMod10 = bigint_utils::getPower10(chunkPwr10);
    
        number_holder_t rem = m_module;
        while(true)
        {
            auto nextModule = moduleDiv(rem, biDividerMod10.m_module);
            shrinkLeadingZeros(nextModule);
            shrinkLeadingZeros(rem);
    
            if (rem.size()>1)
                throw std::runtime_error("BigInt::moduleToStringReversed(): something goes wrong");
    
            if (rem.empty())
                module10.push_back(0u);
            else
                module10.push_back(rem.front());
    
            if (nextModule.empty())
                break;
    
            rem = nextModule;
        }
    
        // 2718121812459045
        // 2718121812459045 % 100 = 45
        // 27181218124590   % 100 = 90
        // 271812181245     % 100 = 45
        // 2718121812       % 100 = 12
        // 27181218         % 100 = 18
        // 271812           % 100 = 12
        // 2718             % 100 = 18
        // 27               % 100 = 27
    
        // У нас остатки от деления на 10^N, начиная с младших разрядов
    
        //std::string resStr; 
        resStr.reserve(std::size_t(chunkPwr10)*module10.size());
    
        for(auto r10: module10)
        {
            for(int i=0; i!=chunkPwr10; ++i)
            {
                resStr.append(1, bigint_utils::digitToChar(int(r10%10), upperCase));
                r10 /= 10;
            }
        }
    }
    else // if (base==8)
    {
        number_holder_t module8; module8.reserve(m_module.size());
    
        const int chunkPwr8 = bigint_utils::getTypeOctalDigits<unsigned_t>();
        const BigInt biDividerMod8 = bigint_utils::getPower8(chunkPwr8);
    
        number_holder_t rem = m_module;
        while(true)
        {
            auto nextModule = moduleDiv(rem, biDividerMod8.m_module);
            shrinkLeadingZeros(nextModule);
            shrinkLeadingZeros(rem);
    
            if (rem.size()>1)
                throw std::runtime_error("BigInt::moduleToStringReversed(): something goes wrong (8)");
    
            if (rem.empty())
                module8.push_back(0u);
            else
                module8.push_back(rem.front());
    
            if (nextModule.empty())
                break;
    
            rem = nextModule;
        }
    
        //std::string resStr; 
        resStr.reserve(std::size_t(chunkPwr8)*module8.size());
    
        for(auto r8: module8)
        {
            for(int i=0; i!=chunkPwr8; ++i)
            {
                resStr.append(1, bigint_utils::digitToChar(int(r8%8), upperCase));
                r8 /= 8;
            }
        }
    }

    while(!resStr.empty() && resStr.back()=='0')
        resStr.pop_back();
     
    if (resStr.empty())
        resStr.append(1, '0');

    return resStr;    
}

//----------------------------------------------------------------------------
inline
std::string BigInt::moduleToString(int base, bool upperCase) const
{
    std::string str = moduleToStringReversed(base, upperCase);
    std::reverse(str.begin(), str.end());
    return str;
}

//----------------------------------------------------------------------------
inline
std::string BigInt::toStringEx(int base, bool upperCase, bool addPrefix) const
{
    auto makeSignStr = [&]()
    {
        return m_sign<0 ? "-" : "";
    };

    if (!addPrefix || base==10)
        return makeSignStr() + moduleToString(base, upperCase);

    if (base!=8)
    {
        return makeSignStr() + std::string(base==2 ? "0xb" : "0x") + moduleToString(base, upperCase);
    }

    if (m_sign!=0)
        return makeSignStr() + std::string("0") + moduleToString(base, upperCase);

    return "0";
}

//----------------------------------------------------------------------------
inline
std::string BigInt::toString() const
{
    return toStringEx(10);

#if 0
    if (m_sign==0)
        return std::string(1, '0');

    number_holder_t module10;

    const int chunkPwr10 = bigint_utils::getTypeDecimalDigits<unsigned_t>();
    const BigInt biDividerMod10 = bigint_utils::getPower10(chunkPwr10);

    number_holder_t rem = m_module;
    while(true)
    {
        auto nextModule = moduleDiv(rem, biDividerMod10.m_module);
        shrinkLeadingZeros(nextModule);
        shrinkLeadingZeros(rem);

        if (rem.size()>1)
            throw std::runtime_error("BigInt::toString(): something goes wrong");

        if (rem.empty())
            module10.push_back(0u);
        else
            module10.push_back(rem.front());

        if (nextModule.empty())
            break;

        rem = nextModule;
    }

    // 2718121812459045
    // 2718121812459045 % 100 = 45
    // 27181218124590   % 100 = 90
    // 271812181245     % 100 = 45
    // 2718121812       % 100 = 12
    // 27181218         % 100 = 18
    // 271812           % 100 = 12
    // 2718             % 100 = 18
    // 27               % 100 = 27

    // У нас остатки от деления на 10^N, начиная с младших разрядов

    std::string resStr; resStr.reserve(std::size_t(chunkPwr10)*module10.size());

    for(auto r10: module10)
    {
        for(int i=0; i!=chunkPwr10; ++i)
        {
            resStr.append(1, char('0' + int(r10%10)));
            r10 /= 10;
        }
    }

    while(!resStr.empty() && resStr.back()=='0')
        resStr.pop_back();

    if (resStr.empty())
        resStr.append(1, '0');
    
    if (m_sign<0)
        resStr.append(1, '-');

    std::reverse(resStr.begin(), resStr.end());

    return resStr;
#endif
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool BigInt::moduleToIntegralConvertionHelper(std::uint64_t &t) const
{
    t = 0;

    if (!m_sign)
        return true; // валидный ноль

    auto module = m_module;
    shrinkLeadingZeros(module);
    if (module.empty())
        return true; // валидный ноль

    const auto maxShiftBitsValue  = int(sizeof(t)*CHAR_BIT);

    const auto shiftBitsStepValue = int(sizeof(unsigned_t)*CHAR_BIT);

    for(std::size_t idx=0u; idx!=module.size(); ++idx)
    {
        const auto curShift = int(shiftBitsStepValue*idx);
        if (curShift>=maxShiftBitsValue)
            return false; // ненулевое значение (а нулевых ведущих у нас нет) сдвигаем за пределы целевого типа - конвертация прошла с усечением.

        const std::uint64_t mi = std::uint64_t(module[idx]); // очередная часть
        const std::uint64_t miShifted = std::uint64_t(mi<<curShift);
        t |= miShifted;
    }

    return true; // текущий модуль влез в целевое значение
}

//----------------------------------------------------------------------------
inline
bool BigInt::moduleToIntegralConvertionHelper(std::int64_t &t) const
{
    t = 0;

    if (!m_sign)
        return true; // валидный ноль

    std::uint64_t t_ = 0;
    if (!moduleToIntegralConvertionHelper(t_))
    {
        // У нас не влезло даже в uint64_t
        if (m_sign<0)
            t = ~std::int64_t(t_) + std::int64_t(1);
        else
            t = std::int64_t(t_);
        return false;
    }

    constexpr const auto int64_max = std::numeric_limits<std::int64_t>::max();
    constexpr const auto uint64_int64_max = static_cast<std::uint64_t>(int64_max);
    constexpr const auto uint64_int64_min_abs = static_cast<std::uint64_t>(int64_max) + 1;

    if (m_sign<0)
    {
        t = - std::int64_t(t_);
        return t_ <= uint64_int64_min_abs;
    }

    t = std::int64_t(t_);
    return t_ <= uint64_int64_max;
}

//----------------------------------------------------------------------------
#if (__cplusplus>=202002L)

    template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
    std::strong_ordering operator<=>(T t, const BigInt &b) const { return BigInt(t).operator<=>(b); }

#endif

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
bool operator==(T t, const BigInt &b) { return BigInt(t).operator==(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
bool operator!=(T t, const BigInt &b) { return BigInt(t).operator!=(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
bool operator<=(T t, const BigInt &b) { return BigInt(t).operator<=(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
bool operator< (T t, const BigInt &b) { return BigInt(t).operator< (b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
bool operator>=(T t, const BigInt &b) { return BigInt(t).operator>=(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
bool operator> (T t, const BigInt &b) { return BigInt(t).operator> (b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator+(T t, const BigInt &b) { return BigInt(t).operator+(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator-(T t, const BigInt &b) { return BigInt(t).operator-(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator*(T t, const BigInt &b) { return BigInt(t).operator*(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator/(T t, const BigInt &b) { return BigInt(t).operator/(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator%(T t, const BigInt &b) { return BigInt(t).operator%(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator&(T t, const BigInt &b) { return BigInt(t).operator&(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator|(T t, const BigInt &b) { return BigInt(t).operator|(b); }

template < typename T, std::enable_if_t< std::is_integral_v<T>, int> = 0 >
BigInt operator^(T t, const BigInt &b) { return BigInt(t).operator^(b); }


//----------------------------------------------------------------------------
inline
std::string to_string(const BigInt& b)
{
    return b.toString();
}

//----------------------------------------------------------------------------
