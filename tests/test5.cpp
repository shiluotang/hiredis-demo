#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <unistd.h>

#include "../src/hiredis.hpp"

int main(int argc, char const* argv[]) try {
    using namespace std;
    using namespace org::sqg;

    std::string const KEY = "loseyourmind";
    std::string const VALUE = "nonsense";

    hiredis redis("localhost", 6379);
    redis.auth("123");

    long cursor = 0L;
    cursor = redis.scan(cursor, "*", 100);
    for (int i = 0; i < 100 && cursor != 0L; ++i)
        cursor = redis.scan(cursor, "460,01,*", 100);

    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
