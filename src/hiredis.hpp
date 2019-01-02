#ifndef HIREDIS_DEMO_HIREDIS_HPP_INCLUDED
#define HIREDIS_DEMO_HIREDIS_HPP_INCLUDED

#include <cstdlib>
#include <string>
#include <map>
#include <ctime>

#include "optional.hpp"

#include <hiredis/hiredis.h>

namespace org {
    namespace sqg {
        class hiredis {
        private:
            hiredis(hiredis const&);
        public:
            hiredis(std::string const &host, unsigned short port);

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

        protected:
            ::redisReply* command(char const *fmt, ...);
        private:
            ::redisContext *_M_ctx;
        public:
            static std::string const OK;
        };
    }
}

#endif // HIREDIS_DEMO_HIREDIS_HPP_INCLUDED
