#pragma once
#include <filesystem>
#include "details/sd_journal.hpp"
#include "journal_entry.hpp"
namespace jrn
{
class JournalLogManager
{
  public:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = JournalEntry;
        using pointer = JournalEntry *;
        using reference = const JournalEntry &;

        Iterator(sd_journal *journal)
            : journal_{journal}
        {
            entry_ = fetch_entry(journal_);
        }

        bool is_end() const
        {
            return is_end_;
        }

        reference operator*() const
        {
            return entry_;
        }
        pointer operator->()
        {
            return std::addressof(entry_);
        }
        Iterator &operator++()
        {
            is_end_ = sd_journal_next(journal_) == 0;
            entry_ = fetch_entry(journal_);
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        friend bool operator==(const Iterator &a, const Iterator &b)
        {
            return sd_id128_equal(a.entry_.id, b.entry_.id) == 1;
        };
        friend bool operator!=(const Iterator &a, const Iterator &b)
        {
            return sd_id128_equal(a.entry_.id, b.entry_.id) == 0;
        };

      private:
        sd_journal *journal_;
        bool is_end_{false};
        JournalEntry entry_;
    };
    explicit JournalLogManager(const std::filesystem::path &file_or_directory);

    Iterator begin()
    {
        sd_journal_seek_head(journal_.get());
        return Iterator{journal_.get()};
    }

    Iterator begin(int offset)
    {
        sd_journal_seek_head(journal_.get());
        sd_journal_next_skip(journal_.get(), offset);
        return Iterator{journal_.get()};
    }

  private:
    systemd::Journal journal_;
    char *cursor_{nullptr};
};
} // namespace jrn
