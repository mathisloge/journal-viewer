#pragma once
#include <asio/awaitable.hpp>

namespace jrn
{
template <typename T>
using task = asio::awaitable<T>;
}
