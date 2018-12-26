#ifndef HIREDIS_DEMO_MISC_HPP_INCLUDED
#define HIREDIS_DEMO_MISC_HPP_INCLUDED

#include <sstream>

namespace org {
    namespace sqg {

        template <typename U, typename V>
        U sstream_cast(V const &val) {
            U u;
            std::stringstream ss;
            ss << val;
            ss >> u;
            return u;
        }
    }
}

#endif // HIREDIS_DEMO_MISC_HPP_INCLUDED
