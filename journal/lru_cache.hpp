#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <fmt/base.h>
#include "journal_entry.hpp"
namespace jrn
{
class LRUCache
{
  public:
    // Konstruktor: capacity = maximale Anzahl an Chunks, chunkSize = Anzahl Einträge pro Chunk.
    explicit LRUCache(std::function<std::vector<JournalEntry>(std::uint64_t start)> load_fnc,
                      size_t capacity,
                      size_t chunk_size)
        : capacity_(capacity)
        , chunk_size_{chunk_size}
        , load_chunk_fnc_{std::move(load_fnc)}
    {}

    const JournalEntry &get_entry(std::uint64_t global_index)
    {
        const size_t chunk_start = (global_index / chunk_size_) * chunk_size_;
        const size_t local_index = global_index % chunk_size_;

        // Check if chunk is at the beginning.
        auto it = cache_map_.find(chunk_start);
        if (it != cache_map_.end())
        {
            // move found chunk to the front
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
            return it->second->chunk[local_index];
        }

        // chunk not found, load it.
        std::vector<JournalEntry> new_chunk = load_chunk(chunk_start);
        lru_list_.push_front(CacheEntry{chunk_start, std::move(new_chunk)});
        cache_map_[chunk_start] = lru_list_.begin();

        fmt::println("load chunk for start {}", chunk_start);

        // remove least recently used entry if capacity is reached.
        if (cache_map_.size() > capacity_)
        {
            auto last = lru_list_.end();
            --last;
            fmt::println("Delete cache entry {}", last->start_index);
            cache_map_.erase(last->start_index);
            lru_list_.pop_back();
        }

        return lru_list_.front().chunk[local_index];
    }

    void clear()
    {
        cache_map_.clear();
        lru_list_.clear();
    }

  private:
    struct CacheEntry
    {
        std::uint64_t start_index;
        std::vector<JournalEntry> chunk;
    };

    size_t capacity_;
    size_t chunk_size_;
    std::list<CacheEntry> lru_list_;
    // Key = Chunk-start_index, Value = Iterator of the LRU list
    std::unordered_map<std::uint64_t, std::list<CacheEntry>::iterator> cache_map_;

    // Platzhaltermethode zum Laden eines Chunks, falls er nicht im Cache ist.
    // Hier als Beispiel: Erzeugt einen Chunk mit dummy JournalEntry-Daten.
    std::vector<JournalEntry> load_chunk(size_t chunk_start)
    {
        return load_chunk_fnc_(chunk_start);
    }

    std::function<std::vector<JournalEntry>(std::uint64_t start)> load_chunk_fnc_;
};
} // namespace jrn
