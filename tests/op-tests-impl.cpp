/*! \file
    \brief Тестим операторы marty::BigInt
 */


#include <array>
#include <iostream>
#include <iomanip>
#include <string>

//
#include "marty_bigint/marty_bigint.h"

#include <windows.h>

#include "marty_bigint/undef_min_max.h"



//----------------------------------------------------------------------------
inline
std::uint32_t getMillisecTick()
{
    #if defined(WIN32) || defined(_WIN32)

        // https://devblogs.microsoft.com/cppblog/visual-c-compiler-version/
        // https://learn.microsoft.com/en-us/cpp/overview/compiler-versions?view=msvc-170

        #if defined(_MSC_VER) && _MSC_VER>1929
            #pragma warning(push)
            #pragma warning(disable:28159) // - warning C28159: Consider using 'GetTickCount64' instead of 'GetTickCount'
        #endif

        return (std::uint32_t)GetTickCount();

        #if defined(_MSC_VER) && _MSC_VER>1929
            #pragma warning(pop)
        #endif

    #else // Linups users can add native millisec counter getter or new std chrono fns

        return 0;

    #endif
}

struct TickElapsedPrinter
{
    std::uint32_t  startTick = 0;
    std::string    msg;

    TickElapsedPrinter(const std::string &m) : startTick(getMillisecTick()), msg(m) {}
    ~TickElapsedPrinter()
    {
        std::cout << msg << ": " << (getMillisecTick()-startTick) << "\n";
    }

}; // struct TickElapsedPrinter




int unsafeMain(int argc, char* argv[]);


int main(int argc, char* argv[])
{
    try
    {
        return unsafeMain(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "unknown error\n";
        return 2;
    }

}

inline
bool isIgnoreResultOpSign(const std::string &s)
{
    return s.size()>=3;
}

inline
std::string removeIgnoreResultFromOpSign(std::string s)
{
    if (!isIgnoreResultOpSign(s))
        return s;
    s.erase(0,3);
    return s;
}


inline
std::string mkMarker(bool bGood, bool bIgnore)
{
    return bIgnore
         ? std::string("[!]") + "   "
         : std::string(bGood ? "[+]" : "[-]") + "   ";
         ;
}

template<typename Op>
bool testBigIntImpl(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2, Op op)
{
    std::int64_t  iRes = 0;
    marty::BigInt bRes = 0;

    std::string opStr = op(iRes, bRes, i1, i2);

    bool bIgnoreRes = isIgnoreResultOpSign(opStr);
    opStr = removeIgnoreResultFromOpSign(opStr);

    if (opStr.empty())
        return true;


    // std::int64_t(i1+i2);
    // marty::BigInt(i1) + marty::BigInt(i2);

    using std::to_string;

    auto iStr  = to_string(iRes);
    auto bStr  = to_string(bRes);

    bool bGood = iStr==bStr;

    std::cout << mkMarker(bGood, bIgnoreRes);
    std::cout << i1 << " " << opStr << " " << i2 << " = " << std::flush;

    if (bIgnoreRes)
    {
        std::cout << bStr;
        std::cout << " - ignored\n" << std::flush;
        bGood = true;
    }
    else
    {
        std::cout << iStr;

        if (bGood)
        {
            std::cout << " - passed\n" << std::flush;
        }
        else
        {
            std::cout << " - failed, result: " << bStr;
            std::cout << " (" << marty::BigInt::getMultiplicationMethodName() << ")\n" << std::flush;
        }
    }

    ++nTotal;

    if (bGood)
       ++nPassed;

    return bGood;
}



inline
bool testBigIntPlus(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        iRes = i1 + i2;
                        bRes = marty::BigInt(i1) + marty::BigInt(i2);
                        return "+";
                    }
                  );
}

inline
bool testBigIntMinus(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        iRes = i1 - i2;
                        bRes = marty::BigInt(i1) - marty::BigInt(i2);
                        return "-";
                    }
                  );
}

inline
bool testBigIntMul(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        iRes = i1 * i2;
                        bRes = marty::BigInt(i1) * marty::BigInt(i2);
                        return "*";
                    }
                  );
}

inline
bool testBigIntDiv(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        if (i2==0)
                            return "---";
                        iRes = i1 / i2;
                        bRes = marty::BigInt(i1) / marty::BigInt(i2);
                        return "/";
                    }
                  );
}

inline
bool testBigIntRem(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        if (i2==0)
                            return "---";
                        iRes = i1 % i2;
                        bRes = marty::BigInt(i1) % marty::BigInt(i2);
                        return "%";
                    }
                  );
}

inline
bool testBigIntShiftLeft(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        i2 = 23;
                        iRes = i1 << 23;
                        bRes = marty::BigInt(i1) << 23;
                        return "<<";
                    }
                  );
}

inline
bool testBigIntShiftRight(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        if (i1<0)
                            return std::string();
                        i2 = 23;
                        iRes = i1 >> 23;
                        bRes = marty::BigInt(i1) >> 23;
                        return ">>";
                    }
                  );
}

inline
bool testBigIntAnd(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        if (i1<0) i1 = -i1;
                        if (i2<0) i2 = -i2;
                        iRes = i1 & i2;
                        bRes = marty::BigInt(i1) & marty::BigInt(i2);
                        return "&";
                    }
                  );
}

inline
bool testBigIntOr(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        if (i1<0) i1 = -i1;
                        if (i2<0) i2 = -i2;
                        iRes = i1 | i2;
                        bRes = marty::BigInt(i1) | marty::BigInt(i2);
                        return "|";
                    }
                  );
}

inline
bool testBigIntXor(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        if (i1<0) i1 = -i1;
                        if (i2<0) i2 = -i2;
                        iRes = i1 ^ i2;
                        bRes = marty::BigInt(i1) ^ marty::BigInt(i2);
                        return "^";
                    }
                  );
}

inline
bool testBigIntInvert(int &nTotal, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    return
    testBigIntImpl( nTotal, nPassed, i1, i2
                  , [](std::int64_t &iRes, marty::BigInt &bRes, std::int64_t &i1, std::int64_t &i2) -> std::string
                    {
                        i2 = 0;
                        iRes = ~i1;
                        bRes = ~marty::BigInt(i1);
                        return "---~";
                    }
                  );
}

inline void testConversions(std::int64_t i)
{
    using std::to_string;

    std::string i64str = to_string(i);

    marty::BigInt bi        = i;
    marty::BigInt biFromStr = i64str;

    std::cout << "Conv test\n"
              << "  i64       : " << i64str        << "\n"
              << "  BI        : " << to_string(bi) << "\n"
              << "  BI FromStr: " << to_string(biFromStr) << "\n"
              ;


#if defined(USE_MARTY_DECIMAL) && USE_MARTY_DECIMAL!=0

    marty::Decimal di  = i;
    marty::Decimal ds  = i64str;
    marty::Decimal dbi = bi;

    std::cout << "  DI        : " << to_string(di )  << "\n"
              << "  DS        : " << to_string(ds )  << "\n"
              << "  DBI       : " << to_string(dbi)  << "\n"
              ;

#endif

    // std::cout << mkMarker(bGood, bIgnoreRes);
    // std::cout << i1 << " " << opStr << " " << i2 << " = " << std::flush;
    //  
    // if (bIgnoreRes)
    // {
    //     std::cout << bStr;
    //     std::cout << " - ignored\n" << std::flush;
    //     bGood = true;
    // }
    // else
    // {
    //     std::cout << iStr;
    //  
    //     if (bGood)
    //     {
    //         std::cout << " - passed\n" << std::flush;
    //     }
    //     else
    //     {
    //         std::cout << " - failed, result: " << bStr;
    //         std::cout << " (" << marty::BigInt::getMultiplicationMethodName() << ")\n" << std::flush;
    //     }
    // }
    //  
    // ++nTotal;
    //  
    // if (bGood)
    //    ++nPassed;
    //  
    // return bGood;


}

inline
void doTestImpl(int &nTest, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    testConversions(i1);
    testConversions(i2);

    testBigIntPlus (nTest, nPassed, i1, i2);
    testBigIntMinus(nTest, nPassed, i1, i2);
    testBigIntMul  (nTest, nPassed, i1, i2);
    testBigIntDiv  (nTest, nPassed, i1, i2);
    testBigIntRem  (nTest, nPassed, i1, i2);
    
    testBigIntShiftLeft(nTest, nPassed, i1, i2);
    testBigIntShiftRight(nTest, nPassed, i1, i2);

    testBigIntAnd   (nTest, nPassed, i1, i2);
    testBigIntOr    (nTest, nPassed, i1, i2);
    testBigIntXor   (nTest, nPassed, i1, i2);
    testBigIntInvert(nTest, nPassed, i1, i2);

}

inline
void doTest(int &nTest, int &nPassed, std::int64_t i1, std::int64_t i2)
{
    doTestImpl(nTest, nPassed,  i1,  i2);
    doTestImpl(nTest, nPassed,  i1, -i2);
    doTestImpl(nTest, nPassed, -i1,  i2);
    doTestImpl(nTest, nPassed, -i1, -i2);
}

// std::string testFormatString(int nTest, const std::string& fmt, const marty::format::Args &args)

// Маска старшего бита
template <typename T>
constexpr auto highest_bit_mask() {
    using UT = typename std::make_unsigned<T>::type;
    return UT{1} << (sizeof(T) * 8 - 1);
}

// Маска N старших битов
template <size_t N, typename T>
constexpr auto high_bits_mask() {
    using UT = typename std::make_unsigned<T>::type;
    constexpr size_t type_bits = sizeof(T) * 8;
    
    if constexpr (type_bits <= N) {
        return ~UT{0};
    } else {
        return ( (UT{1} << N) - UT{1} ) << (type_bits - N);
    }
}

template<typename T>
void printDigits10()
{
    std::cout << "Digits10 for " << typeid(T).name() << ": " << std::numeric_limits<T>::digits10 << " (" << marty::bigint_utils::getTypeDecimalDigits<T>() << ")\n";
}

template<typename T>
void printDigits8()
{
    std::cout << "Digits8 for " << typeid(T).name() << ": " << marty::bigint_utils::getTypeOctalDigits<T>() << "\n";
}



int unsafeMain(int argc, char* argv[])
{

    MARTY_ARG_USED(argc);
    MARTY_ARG_USED(argv);

    using std::to_string;


    #if defined(WIN32) || defined(_WIN32)
        #if defined(WIN64) || defined(_WIN64)
            std::cout<<"Platform: x64" << std::endl << std::flush;
        #else
            std::cout<<"Platform: x86" << std::endl << std::flush;
        #endif
    #else
            std::cout<<"Platform: unknown" << std::endl << std::flush;
    #endif

    std::cout << "BigInt chunk size: " << sizeof(marty::BigInt::chunk_type) << "\n" << std::flush;
    std::cout << "-------------------------\n\n" << std::flush;

    printDigits10<std::uint8_t>();   // 0xFF               - 255                  -  2
    printDigits10<std::uint16_t>();  // 0xFFFF             - 65536                -  4
    printDigits10<std::uint32_t>();  // 0xFFFFFFFF         - 4294967295           -  9
    printDigits10<std::uint64_t>();  // 0xFFFFFFFFFFFFFFFF - 18446744073709551615 - 19

    std::cout << "\n" << std::flush;

    printDigits10<std::uint8_t>();   // 0xFF               - 255                  -  2
    printDigits10<std::uint16_t>();  // 0xFFFF             - 65536                -  5
    printDigits10<std::uint32_t>();  // 0xFFFFFFFF         - 4294967295           - 10
    printDigits10<std::uint64_t>();  // 0xFFFFFFFFFFFFFFFF - 18446744073709551615 - 21

    std::cout << "\n" << std::flush;



    // Добавить тест на переполнение - сложить два минимальных int64_t, сложить их же 
    // как BigInt, потом преобразовать обратно в int64_t и сравнить с оригиналом

    // marty::BigInt bi = -1;
    // bi <<= 23;

    // marty::BigInt bi = 1;
    // bi *= 2;

    using marty::BigInt;

    marty::BigInt biE = marty::BigInt(2718121812459045ull);
    std::cout << "BigInt(2718121812459045): " << to_string(biE) << "\n" << std::flush;
    std::cout << "\n" << std::flush;


    BigInt bi;

    bi = marty::BigInt("125"); // -253 
    std::cout << "marty::BigInt(\"125\"): " << to_string(bi) << "\n" << std::flush;
    std::cout << "\n" << std::flush;
    
    bi = marty::BigInt("0x125"); // -253 
    std::cout << "marty::BigInt(\"0x125\"): " << to_string(bi) << "\n" << std::flush;
    std::cout << "\n" << std::flush;
    
    bi = BigInt(0); 
    std::string biStr;

    std::int64_t int64_max = std::numeric_limits<std::int64_t>::max();
    std::int64_t int64_min = std::numeric_limits<std::int64_t>::min();

    BigInt int64_max_bi = int64_max;
    BigInt int64_min_bi = int64_min;

    std::int64_t int64_max2 = int64_max + int64_max;
    std::int64_t int64_min2 = int64_min + int64_min;

    BigInt int64_max2_bi = int64_max_bi + int64_max_bi;
    BigInt int64_min2_bi = int64_min_bi + int64_min_bi;

    if (int64_max2!=std::int64_t(int64_max2_bi))
        std::cout << "Overflow not match\n" << std::flush;

    if (int64_min2!=std::int64_t(int64_min2_bi))
        std::cout << "Underflow not match\n" << std::flush;

    // BigInt::setMultiplicationMethod(BigInt::MultiplicationMethod::school);

    // bi = BigInt(0x1234) * marty::BigInt(4);
    // biStr = to_string(bi);
    // std::cout << "0x1234 * 4 = " << biStr << "\n\n";

/*
    0x56789ABC  / 0x1234              0x56/0x12=4
    0x1234        4
    0x56789ABC-0x48D00000=0xDA89ABC
    0x0DA89ABC                        0xDA/0x12=0x0C
    0x1234

*/
    // bi = marty::BigInt(0x56789ABC) / marty::BigInt(0x1234); // 0x4C016, but got 0x4090A
    // biStr = to_string(bi);
    // std::cout << "0x56789ABC / 0x1234 = " << biStr << "\n\n";

    // 0x0DA89ABC - 0x0DA70000 = 0x19ABC
    // bi = marty::BigInt(0x56789ABC) % marty::BigInt(0x1234); // 0xA44, but got 0xD000A44
    // biStr = to_string(bi);
    // std::cout << "0x56789ABC / 0x1234 = " << biStr << "\n\n";

    // bi = marty::BigInt(3) / marty::BigInt(2);
    // biStr = to_string(bi);
    // std::cout << "3 / 2 = " << biStr << "\n\n";

    // bi = marty::BigInt(3) % marty::BigInt(2);
    // biStr = to_string(bi);
    // std::cout << "3 % 2 = " << biStr << "\n\n";

    // bi = marty::BigInt(3) % marty::BigInt(2);
    // biStr = to_string(bi);
    // std::cout << "3 % 2 = " << biStr << "\n\n";



    int nTest   = 0;
    int nPassed = 0;

    std::array<BigInt::MultiplicationMethod, 3> mMethods = { BigInt::MultiplicationMethod::school, BigInt::MultiplicationMethod::karatsuba, BigInt::MultiplicationMethod::furer };
    // std::array<BigInt::MultiplicationMethod, 2> mMethods = { BigInt::MultiplicationMethod::school, BigInt::MultiplicationMethod::karatsuba };

    for(auto mm: mMethods)
    {
        BigInt::setMultiplicationMethod(mm);
        std::cout << "\n--- " << BigInt::getMultiplicationMethodName() << "\n";


        doTest(nTest, nPassed,     1,     2);
        doTest(nTest, nPassed,     2,     1);
        doTest(nTest, nPassed,     1,     3);
        doTest(nTest, nPassed,     3,     1);
        doTest(nTest, nPassed,     2,     3);
        doTest(nTest, nPassed,     3,     2);
        doTest(nTest, nPassed,     5,     3);
        doTest(nTest, nPassed,     3,     5);
    
        doTest(nTest, nPassed,   125,   253);
        doTest(nTest, nPassed,   253,   125);
    
        doTest(nTest, nPassed, 41125, 62253);
        doTest(nTest, nPassed, 62253, 41125);

        doTest(nTest, nPassed, 0x56789ABC, 0x1234);

    }

    int nFailed = nTest - nPassed;

    std::cout << "\n\nTotal tests: " << nTest << ", passed: " << nPassed << ", failed: " << nFailed << "\n\n";

    #if 0
    std::cout << "h1: " << highest_bit_mask<std::uint8_t>()  << "\n"; // 128
    std::cout << "h4: " << high_bits_mask<4, std::uint8_t>() << "\n"; // 240
    std::cout << "h8: " << high_bits_mask<8, std::uint8_t>() << "\n"; // 255

    std::cout << "\n\n";

    std::cout << "h1: " << marty::bigint_utils::makeHighBitsMask<1, std::uint16_t>() << "\n"; // 128
    std::cout << "h4: " << marty::bigint_utils::makeHighBitsMask<4, std::uint16_t>() << "\n"; // 240
    std::cout << "h8: " << marty::bigint_utils::makeHighBitsMask<8, std::uint16_t>() << "\n"; // 255
    #endif


    return 0;
}


