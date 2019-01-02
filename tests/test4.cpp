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

    int len = 0;

    hiredis redis("localhost", 6379);
    redis.auth("123");

    len = redis.rpushx(KEY, VALUE);
    if (len != 0)
        throw std::runtime_error("expect size == 0");
    len = redis.rpush(KEY, VALUE);
    len = redis.rpush(KEY, VALUE);
    cout << "len = " << len << endl;
    if (len != 2)
        throw std::runtime_error("expect size == 2");
    cout << "llen(" << KEY << ") = " << redis.llen(KEY) << endl;
    if (redis.llen(KEY) != 2)
        throw std::runtime_error("expect size == 2");
    optional<std::string> const &r = redis.lpop(KEY);
    cout << "lpop = " << r.value() << endl;
    optional<std::string> const &r2 = redis.lpop(KEY);
    cout << "lpop = " << r2.value() << endl;
    cout << "llen(" << KEY << ") = " << redis.llen(KEY) << endl;
    if (redis.llen(KEY) != 0)
        throw std::runtime_error("list should be zero");

    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
