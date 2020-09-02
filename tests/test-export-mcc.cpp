// C headers
#include <cstdlib>
#include <ctime>

// C++ headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

// Platform specific headers
#include <unistd.h>

// Project headers
#include "../src/misc.hpp"
#include "../src/hiredis.hpp"

class scan_iterator:
    public std::iterator<std::input_iterator_tag, std::set<std::string> > {
public:
    // static scan_iterator end;
public:
    explicit scan_iterator(org::sqg::hiredis *hiredis_ptr,
            std::string const &pattern, int count = 10,
            std::string const &cursor = "")
        : _M_hiredis_ptr(hiredis_ptr)
        , _M_cursor(cursor)
        , _M_pattern(pattern)
        , _M_count(count)
        , _M_data() {
    }

    bool operator == (scan_iterator const &other) const {
        return _M_cursor == other._M_cursor;
    }

    bool operator != (scan_iterator const &other) const {
        return !(*this == other);
    }

    std::set<std::string>& operator*() { return _M_data; }

    std::set<std::string>* operator->() { return &_M_data; }

    scan_iterator& operator++() {
        std::string c = _M_cursor != "" ? _M_cursor : "0";
        _M_data.clear();
        _M_cursor = _M_hiredis_ptr->scan(c, _M_pattern, _M_count, _M_data);
        std::clog << "cursor = " << c << " => " << _M_cursor << std::endl;
        return *this;
    }

    scan_iterator operator++(int placeholder) {
        scan_iterator snapshot(*this);
        this->operator++();
        return snapshot;
    }

    static scan_iterator& end() {
        static scan_iterator end(NULL, "", 0, "0");
        return end;
    }
private:
    org::sqg::hiredis   *_M_hiredis_ptr;
    std::string _M_cursor;
    std::string _M_pattern;
    int         _M_count;
    std::set<std::string> _M_data;
};

int main(int argc, char* argv[]) try {
    using namespace org::sqg;
    using namespace std;

    std::string const REDIS_HOST = "";
    int const REDIS_PORT = 6379;
    std::string const REDIS_AUTH = "";

    hiredis redis(REDIS_HOST, REDIS_PORT, 1000L, 5000L);
    if (!REDIS_AUTH.empty())
        redis.auth(REDIS_AUTH);

    std::string const filename = "mcc." + REDIS_HOST + ".txt";
    ofstream ofile(filename.c_str());

    std::string key;
    std::string value;
    for (scan_iterator it(&redis, "mcc,*", 1000000);
            it != scan_iterator::end(); ++it) {
        for (std::set<std::string>::const_iterator cit = it->begin();
                cit != it->end(); ++cit) {
            key = *cit;
            value = redis.get(*cit);
            cout << "[" << REDIS_HOST << "] SET " << key << " " << value << endl;
            ofile << "SET " << key << " " << value << endl;
        }
        // 500 ms
        ::usleep(500 * 1000);
    }
    ofile.close();

    return EXIT_SUCCESS;
} catch (std::exception const &e) {
    std::cerr << "[C++ exception] " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "[C++ exception] <UNKNOWN CAUSE>" << std::endl;
    return EXIT_FAILURE;
}


