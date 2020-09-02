#include <cstdlib>
#include <cstdarg>

#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <hiredis.h>

#include "misc.hpp"
#include "hiredis.hpp"

namespace org {
    namespace sqg {
        std::string const hiredis::OK("OK");
    }
}

namespace {
    class reply {
    private:
        reply(reply const&);
    public:
        explicit reply(redisReply *reply, bool owner = true)
            :_M_reply(reply)
            , _M_owner(owner) {
        }

        virtual ~reply() {
            if (_M_owner) {
                if (_M_reply) {
                    ::freeReplyObject(_M_reply);
                    _M_reply = NULL;
                }
            }
        }

        operator bool() const { return !!_M_reply; }

        std::string str() const {
            return std::string(_M_reply->str, _M_reply->len);
        }

        redisReply const* operator->() const {
            return _M_reply;
        }

        redisReply* operator->() {
            return _M_reply;
        }

        bool isOK() const {
            return _M_reply->type == REDIS_REPLY_STATUS
                && org::sqg::hiredis::OK == _M_reply->str;
        }

        bool isOne() const {
            return _M_reply->type == REDIS_REPLY_INTEGER
                && 1 == _M_reply->integer;
        }

        bool isInteger() const { return _M_reply->type == REDIS_REPLY_INTEGER; }
        bool isString() const { return _M_reply->type == REDIS_REPLY_STRING; }
        bool isStatus() const { return _M_reply->type == REDIS_REPLY_STATUS; }
        bool isArray() const { return _M_reply->type == REDIS_REPLY_ARRAY; }
        bool isError() const { return _M_reply->type == REDIS_REPLY_ERROR; }
        bool isNil() const { return _M_reply->type == REDIS_REPLY_NIL; }

    private:
        redisReply *_M_reply;
        bool        _M_owner;
    };

    ::timeval of_millis(long millis) {
        long seconds = millis / 1000;
        long useconds = (millis % 1000) * 1000;
        ::timeval tv;
        tv.tv_sec = seconds;
        tv.tv_usec = useconds;
        return tv;
    }
}

namespace org {
    namespace sqg {

        struct hiredis::impl_data {

            impl_data(::redisContext *ctx = NULL): _M_ctx(ctx) {}

            virtual ~impl_data() {
                if (_M_ctx) {
                    ::redisFree(_M_ctx);
                    _M_ctx = NULL;
                }
            }

            ::redisContext const* get() const { return _M_ctx; }

            ::redisContext* get() { return _M_ctx; }

            void set(::redisContext *ctx) { _M_ctx = ctx; }

            operator bool() const { return !!_M_ctx; }

            std::string strerror() const { return &_M_ctx->errstr[0]; }

            int errcode() const { return _M_ctx->err; }

            ::redisReply* command(char const *fmt, ...) {
                ::redisReply *r = NULL;
                std::va_list args;
                va_start(args, fmt);
                r = reinterpret_cast< ::redisReply* >(::redisvCommand(_M_ctx,
                            fmt, args));
                va_end(args);
                return r;
            }

            bool set_socket_timeout(long millis) {
                return REDIS_OK == ::redisSetTimeout(_M_ctx, of_millis(millis));
            }

            void check_reply(reply const &r) {
                check_null_reply(r);
                if (r.isError())
                    throw std::runtime_error(r.str());
            }

            void check_null_reply(reply const &r) {
                if (!r) {
                    if (REDIS_OK != this->errcode())
                        throw std::runtime_error(this->strerror());
                    throw std::runtime_error("no redis reply!");
                }
            }

            ::redisContext *_M_ctx;
        };

        hiredis::hiredis(std::string const &host, unsigned short port,
                long connect_timeout, long socket_timeout) {
#if __cplusplus >= 201103L
            std::unique_ptr<impl_data> data;
#else
            std::auto_ptr<impl_data> data;
#endif
            if (connect_timeout > 0)
                data.reset(new impl_data(::redisConnectWithTimeout(
                                host.c_str(), port,
                                of_millis(connect_timeout))));
            else
                data.reset(new impl_data(::redisConnect(host.c_str(), port)));
            if (!*data)
                throw std::runtime_error("failed to connect to "
                        + host + ":" + sstream_cast<std::string>(port));
            if (REDIS_OK != data->errcode())
                throw std::runtime_error(data->strerror());

            if (socket_timeout > 0) {
                if (!data->set_socket_timeout(socket_timeout)) {
                    std::string msg("can't set socket timeout!");
                    if (data->errcode())
                        msg = data->strerror();
                    throw std::runtime_error(msg);
                }
            }
            _M_data.reset(data.release());
        }

        hiredis::~hiredis() {
            _M_data.reset(0);
        }

        hiredis& hiredis::auth(std::string const &password) {
            reply r(_M_data->command("AUTH %s", password.c_str()));
            _M_data->check_reply(r);
            if (!r.isOK())
                throw std::runtime_error(r.str());
            return *this;
        }

        std::map<std::string, std::string> hiredis::hgetall(std::string const& key) {
            std::map<std::string, std::string> m;
            reply r(_M_data->command("HGETALL %s", key.c_str()));
            _M_data->check_reply(r);
            for (size_t i = 0, n = r->elements; i != n; i += 2) {
                reply rkey(r->element[i], false);
                reply rval(r->element[i + 1], false);
                if (!rkey.isString() || !rval.isString())
                    throw std::runtime_error("require string!!!");
                m[rkey.str()] = rval.str();
            }
            return m;
        }

        bool hiredis::exists(std::string const &key) {
            reply r(_M_data->command("EXISTS %s", key.c_str()));
            _M_data->check_reply(r);
            return r.isOne();
        }

        optional<std::string> hiredis::get(std::string const &key) {
            reply r(_M_data->command("GET %s", key.c_str()));
            _M_data->check_reply(r);
            if (r.isNil())
                return optional<std::string>();
            return optional<std::string>(r.str());
        }

        optional<std::string> hiredis::hget(std::string const &key,
                std::string const &field) {
            reply r(_M_data->command("HGET %s %s", key.c_str(), field.c_str()));
            _M_data->check_reply(r);
            if (r.isNil())
                return optional<std::string>();
            return optional<std::string>(r.str());
        }

        hiredis& hiredis::set(std::string const &key, std::string const &value) {
            reply r(_M_data->command("SET %s %s", key.c_str(), value.c_str()));
            _M_data->check_reply(r);
            if (!r.isOK())
                throw std::runtime_error("SET failed on key " + key);
            return *this;
        }

        hiredis& hiredis::hset(std::string const &key, std::string const &field,
                std::string const &value) {
            reply r(_M_data->command("HSET %s %s %s", key.c_str(),
                        field.c_str(), value.c_str()));
            _M_data->check_reply(r);
            return *this;
        }

        bool hiredis::del(std::string const &key) {
            reply r(_M_data->command("DEL %s", key.c_str()));
            _M_data->check_reply(r);
            return r.isOne();
        }

        hiredis& hiredis::expire(std::string const &key, std::time_t duration) {
            reply r(_M_data->command("EXPIRE %s %d", key.c_str(),
                        static_cast<int>(duration)));
            _M_data->check_reply(r);
            if (!r.isOne())
                throw std::runtime_error("EXPIRE failed on key " + key);
            return *this;
        }

        hiredis& hiredis::expireAt(std::string const &key,
                std::time_t timepoint) {
            reply r(_M_data->command("EXPIREAT %s %d", key.c_str(),
                        static_cast<int>(timepoint)));
            _M_data->check_reply(r);
            if (!r.isOne())
                throw std::runtime_error("EXPIREAT failed on key " + key);
            return *this;
        }

        bool hiredis::persist(std::string const &key) {
            reply r(_M_data->command("PERSIST %s", key.c_str()));
            _M_data->check_reply(r);
            return r->integer > 0;
        }

        int hiredis::hincrby(std::string const &key, std::string const &field,
                int delta) {
            reply r(_M_data->command("HINCRBY %s %s %d", key.c_str(),
                        field.c_str(), delta));
            _M_data->check_reply(r);
            return r->integer;
        }

        int hiredis::incrby(std::string const &key, int delta) {
            reply r(_M_data->command("INCRBY %s %d", key.c_str(), delta));
            _M_data->check_reply(r);
            return r->integer;
        }

        int hiredis::ttl(std::string const &key) {
            reply r(_M_data->command("TTL %s", key.c_str()));
            _M_data->check_reply(r);
            return r->integer;
        }

        optional<std::string> hiredis::lpop(std::string const &key) {
            reply r(_M_data->command("LPOP %s", key.c_str()));
            _M_data->check_reply(r);
            if (r.isNil())
                return optional<std::string>();
            return optional<std::string>(r.str());
        }

        int hiredis::lpush(std::string const &key, std::string const &value) {
            reply r(_M_data->command("LPUSH %s %s", key.c_str(),
                        value.c_str()));
            _M_data->check_reply(r);
            return static_cast<int>(r->integer);
        }

        int hiredis::lpushx(std::string const &key, std::string const &value) {
            reply r(_M_data->command("LPUSHX %s %s", key.c_str(),
                        value.c_str()));
            _M_data->check_reply(r);
            return static_cast<int>(r->integer);
        }

        optional<std::string> hiredis::rpop(std::string const &key) {
            reply r(_M_data->command("RPOP %s", key.c_str()));
            _M_data->check_reply(r);
            if (r.isNil())
                return optional<std::string>();
            return optional<std::string>(r.str());
        }

        int hiredis::rpush(std::string const &key, std::string const &value) {
            reply r(_M_data->command("RPUSH %s %s", key.c_str(),
                        value.c_str()));
            _M_data->check_reply(r);
            return static_cast<int>(r->integer);
        }

        int hiredis::rpushx(std::string const &key, std::string const &value) {
            reply r(_M_data->command("RPUSHX %s %s", key.c_str(),
                        value.c_str()));
            _M_data->check_reply(r);
            return static_cast<int>(r->integer);
        }

        int hiredis::llen(std::string const &key) {
            reply r(_M_data->command("LLEN %s", key.c_str()));
            _M_data->check_reply(r);
            return static_cast<int>(r->integer);
        }

        long hiredis::scan(long cursor, std::string const &pattern, int count) {
            reply r(_M_data->command("SCAN %ld MATCH %s COUNT %d", cursor,
                        pattern.c_str(), count));
            _M_data->check_reply(r);
            reply c(r->element[0], false);
            reply a(r->element[1], false);
            std::set<std::string> keys;

            for (int i = 0, n = a->elements; i < n; ++i) {
                std::clog << a->element[i]->str << std::endl;
                keys.insert(a->element[i]->str);
            }
            return sstream_cast<long, std::string>(c->str);
        }

        std::string hiredis::scan(std::string const &cursor,
                std::string const &pattern, int count,
                std::set<std::string> &s) {
            reply r(_M_data->command("SCAN %s MATCH %s COUNT %d",
                        cursor.c_str(), pattern.c_str(), count));
            _M_data->check_reply(r);
            reply c(r->element[0], false);
            reply a(r->element[1], false);
            for (int i = 0, n = a->elements; i < n; ++i)
                s.insert(a->element[i]->str);
            return c->str;
        }

        std::set<std::string> hiredis::keys(std::string const &pattern) {
            reply r(_M_data->command("KEYS %s", pattern.c_str()));
            _M_data->check_reply(r);
            std::set<std::string> s;
            for (int i = 0, n = r->elements; i < n; ++i)
                s.insert(r->element[i]->str);
            return s;
        }
    }
}
