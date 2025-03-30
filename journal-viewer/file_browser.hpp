#pragma once
#include <filesystem>
#include <result.hpp>

namespace jrn
{
Result<std::filesystem::path> choose_directory();
} // namespace jrn
