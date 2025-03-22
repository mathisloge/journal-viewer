#pragma once
#include <filesystem>
#include "details/sd_journal.hpp"
#include "journal_entry.hpp"
namespace jrn
{
class JournalLogManager
{
  public:
    explicit JournalLogManager(const std::filesystem::path &file_or_directory);

    void for_each(std::uint64_t begin, std::uint64_t end, auto &&predicate)
    {
        // TODO: this should better be some function directly for ImGuiListClipper by evaluating the internal state and
        // just only seek the really needed distances and jumping back to the actual used items. The first step always
        // needs some special care, since it is always start=0, end=1 and then calculates the height of the item.
        sd_journal_seek_head(journal_.get());
        sd_journal_next_skip(journal_.get(), begin);
        for (std::uint64_t i = begin; i < end; i++)
        {
            predicate(fetch_entry(journal_.get()));
            if (sd_journal_next(journal_.get()) == 0)
            {
                break;
            }
        }
    }

  private:
    systemd::Journal journal_;
    std::string cursor_;
};
} // namespace jrn
