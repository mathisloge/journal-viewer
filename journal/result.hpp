#pragma once
#include <expected>
#include <string>
namespace jrn
{
template <typename T>
using Result = std::expected<T, std::string>;

constexpr auto make_error_result(auto &&message)
{
    return std::unexpected{std::forward<decltype(message)>(message)};
}
} // namespace jrn
