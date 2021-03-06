// Copyright (C) 2019 tribe29 GmbH - License: GNU General Public License v2
// This file is part of Checkmk (https://checkmk.com). It is subject to the
// terms and conditions defined in the file COPYING, which is part of this
// source code package.

#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <type_traits>

#ifdef CMC
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

namespace mk {
std::string unsafe_tolower(const std::string &str) {
    std::string result = str;
    std::transform(str.begin(), str.end(), result.begin(), ::tolower);
    return result;
}

#ifdef CMC
std::string unsafe_toupper(const std::string &str) {
    std::string result = str;
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return result;
}
#endif

bool starts_with(const std::string &input, const std::string &test) {
    return input.size() >= test.size() &&
           std::equal(test.begin(), test.end(), input.begin());
}

std::vector<std::string> split(const std::string &str, char delimiter) {
    std::istringstream iss(str);
    std::vector<std::string> result;
    std::string field;
    while (std::getline(iss, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}

std::string join(const std::vector<std::string> &values,
                 const std::string &separator) {
    std::string result;
    auto it = values.cbegin();
    auto end = values.cend();
    if (it != end) {
        result.append(*it++);
    }
    while (it != end) {
        result.append(separator).append(*it++);
    }
    return result;
}

std::string lstrip(const std::string &str, const std::string &chars) {
    auto pos = str.find_first_not_of(chars);
    return pos == std::string::npos ? "" : str.substr(pos);
}

std::string rstrip(const std::string &str, const std::string &chars) {
    auto pos = str.find_last_not_of(chars);
    return pos == std::string::npos ? "" : str.substr(0, pos + 1);
}

// TODO (sk): unit tests
std::string_view rstrip(std::string_view str, std::string_view chars) {
    auto pos = str.find_last_not_of(chars);
    return (pos == std::string_view::npos)
               ? std::string_view{str.data(), 0}         // empty
               : std::string_view{str.data(), pos + 1};  // with data
}

// TODO (sk): unit tests
std::string_view lstrip(std::string_view str, std::string_view chars) {
    auto pos = str.find_first_not_of(chars);
    return (pos == std::string_view::npos)
               ? std::string_view{str.data(), 0}
               : std::string_view{str.data() + pos, str.size() - pos};
}

std::string strip(const std::string &str, const std::string &chars) {
    return rstrip(lstrip(str, chars), chars);
}

std::pair<std::string, std::string> nextField(const std::string &str,
                                              const std::string &chars) {
    auto s = lstrip(str, chars);
    auto pos = s.find_first_of(chars);
    return pos == std::string::npos
               ? std::make_pair(s, "")
               : std::make_pair(s.substr(0, pos), s.substr(pos + 1));
}

std::string replace_first(const std::string &str, const std::string &from,
                          const std::string &to) {
    if (str.empty() && from.empty()) {
        return "";
    }
    size_t match = str.find(from);
    if (match == std::string::npos) {
        return str;
    }
    std::string result;
    result.reserve(str.size() + to.size() - from.size());
    return result.append(str, 0, match)
        .append(to)
        .append(str, match + from.size());
}

std::string replace_all(const std::string &str, const std::string &from,
                        const std::string &to) {
    std::string result;
    result.reserve(str.size());
    size_t added_after_match = from.empty() ? 1 : 0;
    size_t pos = 0;
    size_t match = 0;
    while ((match = str.find(from, pos)) != std::string::npos) {
        result.append(str, pos, match - pos)
            .append(to)
            .append(str, pos, added_after_match);
        pos = match + from.size() + added_after_match;
    }
    return result.append(str, pos - added_after_match);
}

std::string from_multi_line(const std::string &str) {
    return replace_all(str, "\n", R"(\n)");
}

std::string to_multi_line(const std::string &str) {
    return replace_all(str, R"(\n)", "\n");
}

#ifdef CMC
std::string ipv4ToString(in_addr_t ipv4_address) {
    char addr_buf[INET_ADDRSTRLEN];
    struct in_addr ia = {ipv4_address};
    inet_ntop(AF_INET, &ia, addr_buf, sizeof(addr_buf));
    return addr_buf;
}

std::string portToString(in_port_t port) {
    // NOLINTNEXTLINE(readability-isolate-declaration)
    return std::to_string(ntohs(port));
}
#endif
}  // namespace mk
