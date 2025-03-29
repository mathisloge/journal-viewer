#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <systemd/sd-journal.h>

namespace jrn
{
class JournalCache
{
  public:
    JournalCache(sd_journal *journal)
        : journal_{journal}
    {
        cache_.reserve(1000);
    }

    void build_initial_cache(sd_journal *journal)
    {
        journal_ = journal;
        cache_.clear();
        const int r = sd_journal_seek_head(journal_);
        if (r < 0)
        {
            return;
        }
        cached_count_ = 0;
        continue_build_cache();
    }

    void update_cache()
    {
        auto last_key = cache_.back();
        const int r = sd_journal_seek_cursor(journal_, last_key.data());
        if (r < 0)
        {
            throw std::runtime_error("Can't seek to cursor");
        }
        continue_build_cache();
    }

    void seek_to_index(const std::uint64_t start_index)
    {
        const std::uint64_t cache_index = start_index / cache_interval_;
        const auto cursor = cursor_at_index(cache_index);
        const int r = sd_journal_seek_cursor(journal_, cursor.data());
        if (r < 0)
        {
            throw std::runtime_error("Can't seek to cursor");
        }
        const std::uint64_t skips = start_index - (cache_index * cache_interval_);
        if (sd_journal_next_skip(journal_, skips) < 0)
        {
            // add log because we can't skip
            return;
        }
    }

    std::uint64_t cached_count() const
    {
        return cached_count_;
    }

  private:
    std::string_view cursor_at_index(const std::uint64_t cache_index) const
    {
        if (cache_index >= static_cast<std::uint64_t>(cache_.size()))
        {
            // Add log to update the cache
            return cache_.back();
        }
        return cache_[cache_index];
    }

  private:
    void continue_build_cache()
    {
        while (sd_journal_next(journal_) > 0)
        {
            if (cached_count_ % cache_interval_ == 0)
            {
                char *cursor_data{nullptr};
                if (sd_journal_get_cursor(journal_, &cursor_data) < 0)
                {
                    // add log because we can't get the cursor to an entry.
                    return;
                }
                cache_.emplace_back(cursor_data);
            }
            ++cached_count_;
        }
    }

  private:
    std::uint64_t cache_interval_{200};
    std::uint64_t cached_count_{};
    sd_journal *journal_;
    std::vector<std::string> cache_;
};
} // namespace jrn
