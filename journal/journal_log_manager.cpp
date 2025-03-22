#include "journal_log_manager.hpp"
namespace jrn
{
JournalLogManager::JournalLogManager(const std::filesystem::path &file_or_directory)
    : journal_{nullptr, sd_journal_close}
{
    if (std::filesystem::is_directory(file_or_directory))
    {
        sd_journal *tmp{nullptr};
        int r = sd_journal_open_directory(&tmp, file_or_directory.c_str(), 0);
        // todo handle r error
        journal_.reset(std::move(tmp));
    }
}
} // namespace jrn
