#include <utility>    // std::move

#include <typeinfo>
#include <iostream>
#include <string_view>


template <class T>
constexpr
std::string_view
type_name()
{
    using namespace std;
#ifdef __clang__
    string_view p = __PRETTY_FUNCTION__;
    return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#  else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#  endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
}
#define print_type(v) std::cout << __LINE__ << ". " #v " = " << type_name<decltype(v)>() << '\n'



      int         func_value()   {                              return     42;           }
      int&        func_ref()     { static       int value = 42; return  value;           }
const int&        func_cref()    { static const int value = 42; return  value;           }
      int *       func_ptr()     { static       int value = 42; return &value;           }
const int *       func_cptr()    { static       int value = 42; return &value;           }
const int * const func_ccptr()   { static const int value = 42; return &value;           }
      int&&       func_rref()    {              int value = 42; return std::move(value); }



int main(int argc, char * argv[])
{
    /*
    {
        auto        v1 = func_value();   // 1.1 - v1 type [+] int
        //auto&       v2 = func_value(); // 1.2 - v2 type [-] int& -- compilation error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
        const auto& v3 = func_value();   // 1.3 - v3 type [+] const int&
        auto&&      v4 = func_value();   // 1.4 - v4 type [+] int&&
        
        print_type(v1);
        print_type(v3);
        print_type(v4);
    }
    //*/
    /*
    {
              auto   v1 = func_ref(); // 2.1 - v1 type [+] int
              auto&  v2 = func_ref(); // 2.2 - v2 type [+] int&
        const auto&  v3 = func_ref(); // 2.3 - v3 type [+] const int&
              auto&& v4 = func_ref(); // 2.4 - v4 type [+] int&
        
        print_type(v1);
        print_type(v2);
        print_type(v3);
        print_type(v4);
    }
    //*/
    /*
    {
              auto   v1 = func_cref(); // 3.1 - v1 type [+] int
              auto&  v2 = func_cref(); // 3.2 - v2 type [+] const int&
        const auto&  v3 = func_cref(); // 3.3 - v3 type [+] const int&
              auto&& v4 = func_cref(); // 3.4 - v4 type [+] const int&
        
        print_type(v1);
        print_type(v2);
        print_type(v3);
        print_type(v4);
    }
    //*/
    /*
    {
              auto   v1 = func_ptr();    // 4.1 - v1 type [+] int*
        //    auto&  v2 = func_ptr();    // 4.2 - v2 type [-] int* -- compilation error: cannot bind non-const lvalue reference of type 'int*&' to an rvalue of type 'int*'
        const auto&  v3 = func_ptr();    // 4.3 - v3 type [-] const int* -- int * const&
              auto&& v4 = func_ptr();    // 4.4 - v4 type [-] int* -- int*&&

        print_type(v1);
        //print_type(v2);
        print_type(v3);
        print_type(v4);
    }
    //*/
    /*
    {
              auto   v1 = func_cptr(); // 5.1 - v1 type [+] const int*
        //    auto&  v2 = func_cptr(); // 5.2 - v2 type [+] compilation error
        const auto&  v3 = func_cptr(); // 5.3 - v3 type [+] const int* const
              auto&& v4 = func_cptr(); // 5.4 - v4 type [+] const int*&& 

        print_type(v1);
        //print_type(v2);
        print_type(v3);
        print_type(v4);
    }
    //*/
    /*
    {
              auto   v1 = func_ccptr(); // 6.1 - v1 type [+] const int*
        //    auto&  v2 = func_ccptr(); // 6.2 - v2 type [+] compilation error
        const auto&  v3 = func_ccptr(); // 6.3 - v3 type [+] const inst* const&
              auto&& v4 = func_ccptr(); // 6.4 - v4 type [+] const int*&&

        print_type(v1);
        //print_type(v2);
        print_type(v3);
        print_type(v4);
    }
    //*/
    /*
    {
              auto   v1 = func_rref(); // 7.1 - v1 type [+] int
        //    auto&  v2 = func_rref(); // 7.2 - v2 type [+] compilation error
        const auto&  v3 = func_rref(); // 7.3 - v3 type [+] const int&
              auto&& v4 = func_rref(); // 7.4 - v4 type [+] int&&
        
        print_type(v1);
        //print_type(v2);
        print_type(v3);
        print_type(v4);
    }
    //*/
    //*
    {
        auto v1(10);              // 8.1 - v1 type [+] int
        auto v2 = 10;             // 8.2 - v2 type [+] int
        auto v3{10};              // 8.3 - v3 type [+] int
        auto v4 = {10};           // 8.4 - v4 type [+] std::initializer_list<int>
        auto v5 = {1, 2, 3};      // 8.5 - v5 type [+] std::initializer_list<int>
        //auto v6 = {1, 2, 3.1};  // 8.6 - v6 type [-] std::initializer_list<double> -- compilation error: deduced conflicting types for parameter 'auto' ('int' and 'double')

        print_type(v1);
        print_type(v2);
        print_type(v3);
        print_type(v4);
        print_type(v5);
        //print_type(v6);
    }
    //*/
    return 0;
}
