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
    ::useconds_t const secs = 1000 * 500;
    int ttl = 0;

    hiredis redis("localhost", 6379);
    redis.auth("123");
    redis.set(KEY, VALUE);
    redis.expire(KEY, 3);
    do {
        ::usleep(secs);
        ttl = redis.ttl(KEY);
        cout << "ttl = " << ttl << endl;
    } while (ttl != -2);

    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
