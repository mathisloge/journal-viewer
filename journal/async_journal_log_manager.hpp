// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <mutex>
#include <shared_mutex>
#include <asio/any_io_executor.hpp>
#include <asio/strand.hpp>
#include <journal/lru_cache.hpp>
#include "journal_log_manager.hpp"

namespace jrn
{
class AsyncJournalLogManager
{
  public:
    explicit AsyncJournalLogManager(asio::io_context &io_ctx, JournalInstanceHandle handle);

    void for_each(std::uint64_t begin, const std::uint64_t end, auto &&predicate)
    {
        const auto chunk_start = (begin / chunck_size_);
        const auto chunk_end = (end / chunck_size_);

        for (auto i = chunk_start; i <= chunk_end; i++)
        {
            const auto local_index = begin % chunck_size_;
            const auto end_local_index = std::min(end % chunck_size_, chunck_size_);
            iterate_chunk(i, local_index, end_local_index, predicate);
            begin += end_local_index;
        }
    }

    void reset_filters()
    {
        asio::post(strand_, [this]() {
            manager_.reset_filters();
            chunk_cache_.clear();
        });
    }

    void enable_priority(Priority priority)
    {
        asio::post(strand_, [this, priority]() {
            manager_.enable_priority(priority);
            chunk_cache_.clear();
        });
    }

    void disable_priority(Priority priority)
    {
        asio::post(strand_, [this, priority]() {
            manager_.disable_priority(priority);
            chunk_cache_.clear();
        });
    }

    void update_highlighter_search_text(std::string search_text)
    {
        asio::post(strand_, [this, search_text = std::move(search_text)]() {
            manager_.update_highlighter_search_text(std::move(search_text));
            chunk_cache_.clear();
        });
    }

    void update_exclude_message_regex(std::string exclude_text)
    {
        asio::post(strand_, [this, exclude_text = std::move(exclude_text)]() {
            manager_.update_exclude_message_regex(std::move(exclude_text));
            chunk_cache_.clear();
        });
    }

    void add_filter_systemd_unit(std::string systemd_unit)
    {
        asio::post(strand_, [&]() {
            manager_.add_filter_systemd_unit(systemd_unit);
            chunk_cache_.clear();
        });
    }

    void remove_filter_systemd_unit(const std::string &systemd_unit)
    {
        asio::post([&]() {
            manager_.remove_filter_systemd_unit(systemd_unit);
            chunk_cache_.clear();
        });
    }

    [[nodiscard]] bool has_filter_systemd_unit(const std::string &systemd_unit) const
    {
        return manager_.has_filter_systemd_unit(systemd_unit);
    }

    [[nodiscard]] bool is_priority_enabled(const Priority priority) const
    {
        return manager_.is_priority_enabled(priority);
    }

    std::uint64_t min_count_entries() const
    {
        return manager_.min_count_entries();
    }

    std::uint64_t calculate_cursor_index(std::string_view cursor)
    {
        return 0;
        return manager_.calculate_cursor_index(cursor);
    }

  private:
    void iterate_placeholders(std::uint64_t begin, const std::uint64_t end, auto &&predicate)
    {
        for (; begin < end; begin++)
        {
            predicate(begin, JournalEntry{});
        }
    }

    void iterate_chunk(const std::uint64_t chunk_index,
                       std::uint64_t local_begin,
                       const std::uint64_t local_end,
                       auto &&predicate)
    {
        if (chunk_fetch_operations_.contains(chunk_index))
        {
            iterate_placeholders(local_begin, local_end, predicate);
            return;
        }

        std::shared_lock l{mtx_};
        auto chunk = chunk_cache_.get(chunk_index);
        if (chunk.has_value())
        {
            for (auto i = local_begin; (i < local_end && i < chunk.value()->size()); i++)
            {
                const auto &entry = chunk.value()->at(i);
                predicate(i, entry);
            }
        }
        else
        {
            asio::post(strand_, [this, chunk_index]() {
                chunk_fetch_operations_.emplace(chunk_index);
                const auto chunk_global_index = chunk_index * chunck_size_;
                auto chunk = manager_.fetch_chunk(chunk_global_index, chunk_global_index + chunck_size_);
                {
                    std::unique_lock l{mtx_};
                    chunk_cache_.emplace(chunk_index, std::move(chunk));
                }
                chunk_fetch_operations_.erase(chunk_index);
            });
            iterate_placeholders(local_begin, local_end, std::forward<decltype(predicate)>(predicate));
        }
    }

  private:
    std::shared_mutex mtx_;
    asio::strand<asio::any_io_executor> strand_;
    JournalLogManager manager_;
    LruCache<std::uint64_t, std::vector<JournalEntry>> chunk_cache_;
    std::uint64_t chunck_size_{512};

    std::unordered_set<std::uint64_t> chunk_fetch_operations_;
};
} // namespace jrn
