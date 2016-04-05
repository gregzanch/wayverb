#pragma once

#include "string_builder.h"
#include "extended_algorithms.h"

#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"

#include <set>

std::ostream& operator<<(std::ostream& os, const cl_float3& f);

inline std::ostream& to_stream_spaced(std::ostream& os) {
    return os;
}

template <typename T, typename... Ts>
inline std::ostream& to_stream_spaced(std::ostream& os,
                                      const T& t,
                                      Ts&&... ts) {
    to_stream(os, t, "  ");
    return to_stream_spaced(os, std::forward<Ts>(ts)...);
}

struct to_stream_spaced_functor {
    to_stream_spaced_functor(std::ostream& os)
            : os(os) {
    }
    template <typename... Ts>
    void operator()(Ts&&... ts) const {
        to_stream_spaced(os, std::forward<Ts>(ts)...);
    }
    std::ostream& os;
};

namespace std {

template <typename T, typename U>
inline std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
    Bracketer bracketer(os);
    return to_stream_spaced(os, p.first, p.second);
}

template <typename T, unsigned long U>
inline std::ostream& operator<<(std::ostream& os, const std::array<T, U>& t) {
    Bracketer bracketer(os);
    proc::invoke(to_stream_spaced_functor(os), t);
    return os;
}

template <typename... Ts>
inline std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& t) {
    Bracketer bracketer(os);
    proc::invoke(to_stream_spaced_functor(os), t);
    return os;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& t) {
    Bracketer bracketer(os);
    for (const auto& i : t)
        to_stream_spaced(os, i);
    return os;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::set<T>& t) {
    Bracketer bracketer(os);
    for (const auto& i : t)
        to_stream_spaced(os, i);
    return os;
}
}