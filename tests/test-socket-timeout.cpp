#include <cstdlib>
#include <ctime>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include <unistd.h>

#include "../src/misc.hpp"
#include "../src/hiredis.hpp"

int main(int argc, char* argv[]) try {
    using namespace org::sqg;
    using namespace std;

    hiredis redis("localhost", 6379, 1000L, 200L);
    redis.auth("123");
    for (int i = 0; i < 1000000; ++i)
        redis.set(sstream_cast<std::string>(i), "yes");

    std::time_t t1 = std::time(NULL);
    cout << redis.keys("*").size() << endl;
    std::time_t t2 = std::time(NULL);

    cout << "consumed " << std::difftime(t2, t1) << " seconds" << endl;

    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}

