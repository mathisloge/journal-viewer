#include "file_browser.hpp"
#include <nfd.hpp>

namespace jrn
{
std::expected<std::filesystem::path, std::string> choose_directory()
{
    NFD::UniquePathN path;
    const auto result = NFD::PickFolder(path);
    if (result != NFD_OKAY)
    {
        return std::unexpected("Filedialog aborted");
    }
    return std::filesystem::path{path.get()};
}
} // namespace jrn
