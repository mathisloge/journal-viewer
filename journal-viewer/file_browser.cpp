#include "file_browser.hpp"
#include <nfd.hpp>

namespace jrn
{
std::optional<std::filesystem::path> choose_directory()
{
    NFD::UniquePathN path;
    const auto result = NFD::PickFolder(path);
    if (result != NFD_OKAY)
    {
        return std::nullopt;
    }
    return std::filesystem::path{path.get()};
}
} // namespace jrn
