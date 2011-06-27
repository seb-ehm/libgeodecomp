#ifndef _libgeodecomp_misc_stringops_h_
#define _libgeodecomp_misc_stringops_h_

#include <sstream>

// sad but true: CodeGear's C++ compiler has troubles with the +
// operator for strings.
#ifdef __CODEGEARC__
namespace LibGeoDecomp {
std::string operator+(const std::string& a, const std::string& b)
{
    return std::string(a).append(b);
}

}
#endif

namespace LibGeoDecomp {

class StringConv
{
public:
    static std::string itoa(int i)
    {
        std::stringstream z;
        z << i;
        return z.str();
    }

    static int atoi(std::string s)
    {
        return atoi(s.c_str());
    }
};

}
 
#endif
