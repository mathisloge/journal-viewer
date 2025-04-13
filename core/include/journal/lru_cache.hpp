// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <list>
#include <optional>
#include <unordered_map>
namespace jrn
{
// A least recently used cache.
template <class Key, class Value>
class LruCache
{
  public:
    using key_type = Key;
    using value_type = Value;
    using list_type = std::list<key_type>;
    using map_type = std::unordered_map<key_type, std::pair<value_type, typename list_type::iterator>>;

    LruCache(size_t capacity)
        : capacity_{capacity}
    {}

    ~LruCache() = default;

    size_t size() const
    {
        return map_.size();
    }

    size_t capacity() const
    {
        return capacity_;
    }

    bool empty() const
    {
        return map_.empty();
    }

    bool contains(const key_type &key) const
    {
        return map_.find(key) != map_.end();
    }

    void emplace(const key_type &key, value_type &&value)
    {
        typename map_type::iterator i = map_.find(key);
        if (i == map_.end())
        {
            // is full?
            if (size() >= capacity_)
            {
                remove_least_used();
            }
            list_.emplace_front(key);
            map_[key] = std::make_pair(std::move(value), list_.begin());
        }
    }

    std::optional<value_type *> get(const key_type &key)
    {
        // lookup value in the cache
        const auto key_it = map_.find(key);
        if (key_it == map_.end())
        {
            // value not in cache
            return std::nullopt;
        }

        // update item as most recently
        list_.splice(list_.begin(), list_, key_it->second.second);
        // update map iterator
        key_it->second.second = list_.begin();
        return std::addressof(key_it->second.first);
    }

    void clear()
    {
        map_.clear();
        list_.clear();
    }

  private:
    void remove_least_used()
    {
        auto i = --list_.end();
        map_.erase(*i);
        list_.erase(i);
    }

  private:
    map_type map_;
    list_type list_;
    size_t capacity_;
};
} // namespace jrn
