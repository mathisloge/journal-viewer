#pragma once
#include <filesystem>
#include <optional>

namespace jrn
{
std::optional<std::filesystem::path> choose_directory();
} // namespace jrn
