#include <string>
#include <tuple>
#include <cassert>

#include <iostream>     // std::cout


// ===[ custom_tuple ]==========================================================
template <typename T, typename ...Types>
struct custom_tuple : custom_tuple<Types...>
{
    using parent_t = custom_tuple<Types...>;

    custom_tuple(T elem_, Types... o)
        : parent_t(o...)
        , elem(elem_)
    {}

    template<typename OT, typename ...OTypes>
    custom_tuple& operator=(custom_tuple<OT, OTypes...>&& o)
    {
        if (static_cast<void*>(this) != static_cast<void*>(&o))
        {
            elem = std::move(o.elem);
            static_cast<parent_t&>(*this) = std::move(
                static_cast<custom_tuple<OTypes...>&>(o));
        }
        return *this;
    }

    T elem;
};

template <typename T>
struct custom_tuple<T>
{
    custom_tuple(T elem_) : elem(elem_) {}

    template <typename OT>
    custom_tuple& operator=(custom_tuple<OT>&& o)
    {
        if (static_cast<void*>(this) != static_cast<void*>(&o))
        {
            elem = std::move(o.elem);
        }
        return *this;
    }

    T elem;
};


// ===[ make_custom_tuple ]=====================================================
template <class T>
struct unwrap_refwrapper
{
    using type = T;
};
 
template <class T>
struct unwrap_refwrapper<std::reference_wrapper<T>>
{
    using type = T&;
};
 
template <class T>
using special_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;
 
template <class... Types>
auto make_custom_tuple(Types&&... args)
{
    return custom_tuple<special_decay_t<Types>...>(std::forward<Types>(args)...);
}





#define ENABLE_CUSTOM_TUPLE


// ===[ custom_tie ]============================================================
template <typename ...Types>
auto custom_tie(Types&... args)
{
#ifdef ENABLE_CUSTOM_TUPLE
    return custom_tuple<Types&...>(args...);
#else
    return std::tuple<Types&...>(args...);
#endif
}


// ===[ main and others ]=======================================================
auto getPerson()
{
    const std::string name = "Petia";
    const std::string secondName = "Ivanoff";
    const std::size_t age = 23;
    const std::string department = "Sale";
#ifdef ENABLE_CUSTOM_TUPLE
    return make_custom_tuple(name, secondName, age, department);
#else
    return std::make_tuple(name, secondName, age, department);
#endif
}


int main(int argc, char * argv[])
{
    std::string name, secondName, department;
    std::size_t age;
    custom_tie(name, secondName, age, department) = getPerson();
    assert(name == "Petia");
    assert(secondName == "Ivanoff");
    assert(age == 23);
    assert(department == "Sale");



    
#ifdef ENABLE_CUSTOM_TUPLE
    // Movability presentation
    std::string str;
    int         val  = 0;
    size_t      step = 0;
    auto print_vars = [&]()
    {
        std::cout << "Step " << step << '\n';
        std::cout << "    str = [" << str << "]\n";
        std::cout << "    val = [" << val << "]\n";
    };
    auto ct_1 = make_custom_tuple(std::string("Hello"), 1);
    auto ct_2 = make_custom_tuple(std::string("World"), 2);

    custom_tie(str, val) = std::move(ct_1);
    assert(str == "Hello");
    assert(val == 1);
    step = 1; print_vars();

    custom_tie(str, val) = std::move(ct_2);
    assert(str == "World");
    assert(val == 2);
    step = 2; print_vars();

    custom_tie(str, val) = std::move(ct_1);
    step = 3; print_vars();
#endif
    return 0;
}
