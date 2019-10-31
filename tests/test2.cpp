#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "../src/hiredis.hpp"

int main(int argc, char const* argv[]) try {
    using namespace std;
    using namespace org::sqg;

    std::string const KEY = "ni:289942";

    hiredis redis("localhost", 6379);
    redis.auth("123");

    redis.hset(KEY, "data", "some");
    redis.expire(KEY, static_cast<std::time_t>(1));

    if (!redis.persist(KEY))
        throw std::runtime_error("No exists or no timeout settings");
    redis.hincrby(KEY, "responses", 1);
    optional<std::string> const &value = redis.hget(KEY, "data");
    // deserialize
    cout << value << endl;

    redis.del("A");
    redis.set("A", "");
    optional<std::string> r = redis.get("A");
    if (!r.exists())
        throw std::runtime_error("key \"A\" does not exist!");
    cout << "[" << r << "]" << endl;

    redis.del("A");
    redis.del(KEY);
    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
