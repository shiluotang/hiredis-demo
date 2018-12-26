#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <sstream>
#include <ctime>

#include <unistd.h>

#include "../src/misc.hpp"
#include "../src/hiredis.hpp"

int main(int argc, char* argv[]) try {
    using namespace org::sqg;
    using namespace std;

    std::string const KEY = "loseyourmind";
    std::string const VALUE = "nonsense";

    hiredis redis("localhost", 6379);
    redis.auth("123");
    map<string, string> const &almanacs = redis.hgetall("Key_gpsAlm");
    for (map<string, string>::const_iterator it = almanacs.begin(); it != almanacs.end(); ++it)
        cout << it->first << " = " << it->second << endl;
    redis.set(KEY, VALUE).expire(KEY, static_cast<std::time_t>(1));
    redis.get(KEY);
    redis.persist(KEY);
    sleep(1);
    redis.get(KEY);

    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}
