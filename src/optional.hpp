#ifndef HIREDIS_DEMO_OPTIONAL_HPP_INCLUDED
#define HIREDIS_DEMO_OPTIONAL_HPP_INCLUDED

#include <stdexcept>
#include <ostream>

namespace org {
    namespace sqg {
        template <typename T>
        class optional {
        public:
            optional(T const &value) :_M_present(true), _M_data(value) { }

            optional() :_M_present(false) { }

            operator T() const {
                if (!_M_present)
                    throw std::runtime_error("No value present!");
                return _M_data;
            }

            operator T() {
                if (!_M_present)
                    throw std::runtime_error("No value present!");
                return _M_data;
            }

            bool exists() const { return _M_present; }

            T const& value() const { return _M_data; }

            T& value() { return _M_data; }

            optional<T>& unset() { _M_present = false; }
        private:
            bool _M_present;
            T _M_data;
        };

        template <typename T>
        std::ostream& operator << (std::ostream &os, optional<T> const &val) {
            return os << val.value();
        }
    }
}

#endif // HIREDIS_DEMO_OPTIONAL_HPP_INCLUDED
