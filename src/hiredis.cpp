#include <cstdlib>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <stdexcept>

#include "misc.hpp"
#include "hiredis.hpp"

namespace org {
    namespace sqg {
        std::string const hiredis::OK("OK");
    }
}

namespace {
    class reply {
    public:
        reply(redisReply *reply, bool owner = true) :_M_reply(reply), _M_owner(owner) {
            if (!reply)
                throw std::runtime_error("nil reply");
        }

        virtual ~reply() {
            if (_M_owner) {
                if (_M_reply) {
                    ::freeReplyObject(_M_reply);
                    _M_reply = NULL;
                }
            }
        }

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
            return _M_reply->type == REDIS_REPLY_STATUS && org::sqg::hiredis::OK == _M_reply->str;
        }

        bool isOne() const {
            return _M_reply->type == REDIS_REPLY_INTEGER && 1 == _M_reply->integer;
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
}

namespace org {
    namespace sqg {
        hiredis::hiredis(std::string const &host, unsigned short port)
            : _M_ctx(NULL) {
            _M_ctx = ::redisConnect(host.c_str(), static_cast<int>(port));
            if (!_M_ctx)
                throw std::runtime_error("failed to connect to " + host + ":" + sstream_cast<std::string>(port));
            if (_M_ctx->err) {
                std::string msg(&_M_ctx->errstr[0]);
                ::redisFree(_M_ctx);
                _M_ctx = NULL;
                throw std::runtime_error(msg);
            }
        }

        hiredis::~hiredis() {
            if (_M_ctx) {
                ::redisFree(_M_ctx);
                _M_ctx = NULL;
            }
        }

        hiredis& hiredis::auth(std::string const &password) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "AUTH %s", password.c_str()));
            if (r.isError() || !r.isOK())
                throw std::runtime_error(r.str());
            return *this;
        }

        std::map<std::string, std::string> hiredis::hgetall(std::string const& key) {
            std::map<std::string, std::string> m;
            reply r((redisReply*) ::redisCommand(_M_ctx, "HGETALL %s", key.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
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
            reply r((redisReply*) ::redisCommand(_M_ctx, "EXISTS %s", key.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            return r.isOne();
        }

        optional<std::string> hiredis::get(std::string const &key) {
            optional<std::string> val;
            reply r((redisReply*) ::redisCommand(_M_ctx, "GET %s", key.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            if (r.isNil())
                return optional<std::string>();
            return optional<std::string>(r.str());
        }

        optional<std::string> hiredis::hget(std::string const &key, std::string const &field) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "HGET %s %s", key.c_str(), field.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            if (r.isNil())
                return optional<std::string>();
            return optional<std::string>(r.str());
        }

        hiredis& hiredis::set(std::string const &key, std::string const &value) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "SET %s %s", key.c_str(), value.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            if (!r.isOK())
                throw std::runtime_error("SET failed on key " + key);
            return *this;
        }

        hiredis& hiredis::hset(std::string const &key, std::string const &field, std::string const &value) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            return *this;
        }

        bool hiredis::del(std::string const &key) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "DEL %s", key.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            return r.isOne();
        }

        hiredis& hiredis::expire(std::string const &key, std::time_t duration) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "EXPIRE %s %d", key.c_str(), static_cast<int>(duration)));
            if (r.isError())
                throw std::runtime_error(r.str());
            if (!r.isOne())
                throw std::runtime_error("EXPIRE failed on key " + key);
            return *this;
        }

        hiredis& hiredis::expireAt(std::string const &key, std::time_t timepoint) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "EXPIREAT %s %d", key.c_str(), static_cast<int>(timepoint)));
            if (r.isError())
                throw std::runtime_error(r.str());
            if (!r.isOne())
                throw std::runtime_error("EXPIREAT failed on key " + key);
            return *this;
        }

        bool hiredis::persist(std::string const &key) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "PERSIST %s", key.c_str()));
            if (r.isError())
                throw std::runtime_error(r.str());
            return r->integer > 0;
        }

        int hiredis::hincrby(std::string const &key, std::string const &field, int delta) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "HINCRBY %s %s %d", key.c_str(), field.c_str(), delta));
            if (r.isError())
                throw std::runtime_error(r.str());
            return r->integer;
        }

        int hiredis::incrby(std::string const &key, int delta) {
            reply r((redisReply*) ::redisCommand(_M_ctx, "INCRBY %s %d", key.c_str(), delta));
            if (r.isError())
                throw std::runtime_error(r.str());
            return r->integer;
        }
    }
}
