#pragma once
#include "details/sd_journal.hpp"
#include "journal_cache.hpp"
#include "journal_entry.hpp"
#include "journal_instance_handle.hpp"

namespace jrn
{
class JournalLogManager
{
  public:
    explicit JournalLogManager(JournalInstanceHandle handle);

    void for_each(std::uint64_t begin, std::uint64_t end, auto &&predicate)
    {
        cache_.seek_to_index(begin);
        for (std::uint64_t i = begin; i < end; ++i)
        {
            predicate(fetch_entry(journal_.get()));
            if (sd_journal_next(journal_.get()) == 0)
            {
                break;
            }
        }
    }

    std::uint64_t min_count_entries() const
    {
        return cache_.cached_count();
    }

  private:
    systemd::Journal journal_;
    JournalCache cache_{journal_.get()};
    std::string cursor_;
};
} // namespace jrn
