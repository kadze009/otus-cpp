#include <vector>
#include <list>
#include <set>

#include "print_ip.hpp"




int main()
{
    print_ip(char(-1));
    print_ip(short(0));
    print_ip(int(2'130'706'433));
    print_ip(long(8'875'824'491'850'138'409));

    print_ip("1.1.1.1");
    print_ip(std::string{"1.1.1.2"});
    print_ip(std::string_view{"1.1.1.3"});

    { std::vector<int>    v = {1,1,2,1};   print_ip(v); }
    { std::vector<size_t> v = {1,1,2,2,2}; print_ip(v); }
    { std::list<int>      l = {1,1,3,1};   print_ip(l); }
    { std::set<int>       s = {1,2,3,4,5}; print_ip(s); }

    { auto t = std::make_tuple(1,3,1,1);   print_ip(t); }
    //{ auto t = std::make_tuple(1,3,1u,2);  print_ip(t); }
    //{ auto t = std::make_tuple(1,3,2,3,3); print_ip(t); }

    return 0;
}

