#pragma once
#include <expected>
#include <filesystem>

namespace jrn
{
std::expected<std::filesystem::path, std::string> choose_directory();
} // namespace jrn
