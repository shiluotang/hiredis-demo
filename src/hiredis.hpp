#ifndef HIREDIS_DEMO_HIREDIS_HPP_INCLUDED
#define HIREDIS_DEMO_HIREDIS_HPP_INCLUDED

#include <cstdlib>
#include <ctime>

#include <string>
#include <map>
#include <memory>
#include <set>

#include "optional.hpp"

namespace org {
    namespace sqg {
        class hiredis {
        private:
            hiredis(hiredis const&);
        public:
            hiredis(std::string const &host, unsigned short port,
                    long connect_timeout_millis = 0,
                    long socket_timeout_millis = 0);

            virtual ~hiredis();

            hiredis& auth(std::string const &password);

            std::map<std::string, std::string> hgetall(std::string const& key);

            bool exists(std::string const &key);

            optional<std::string> get(std::string const &key);

            optional<std::string> hget(std::string const &key, std::string const &field);

            hiredis& set(std::string const &key, std::string const &value);

            hiredis& hset(std::string const &key, std::string const& field, std::string const &value);

            bool del(std::string const &key);

            hiredis& expire(std::string const &key, std::time_t duration);

            hiredis& expireAt(std::string const &key, std::time_t timepoint);

            bool persist(std::string const &key);

            int hincrby(std::string const &key, std::string const &field, int delta);

            int incrby(std::string const &key, int delta);

            int ttl(std::string const &key);

            optional<std::string> lpop(std::string const &key);

            int lpush(std::string const &key, std::string const &value);

            int lpushx(std::string const &key, std::string const &value);

            optional<std::string> rpop(std::string const &key);

            int rpush(std::string const &key, std::string const &value);

            int rpushx(std::string const &key, std::string const &value);

            int llen(std::string const &key);

            long scan(long cursor, std::string const &pattern, int count);

            std::set<std::string> keys(std::string const &pattern);

        private:
            struct impl_data;

#if __cplusplus >= 201103L
            std::unique_ptr<impl_data> _M_data;
#else
            std::auto_ptr<impl_data> _M_data;
#endif

        public:
            static std::string const OK;
        };
    }
}

#endif // HIREDIS_DEMO_HIREDIS_HPP_INCLUDED
