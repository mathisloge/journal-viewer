#pragma once
#include <expected>
#include <filesystem>
#include "details/sd_journal.hpp"
#include "journal_instance_handle.hpp"

namespace jrn
{
class JournalInstance
{
  public:
    static std::expected<JournalInstanceHandle, std::string> from_directory(std::filesystem::path directory);

    JournalInstance(const JournalInstance &) = delete;
    JournalInstance(JournalInstance &&) noexcept = delete;
    JournalInstance &operator=(const JournalInstance &) = delete;
    JournalInstance &operator=(JournalInstance &&) noexcept = delete;

    virtual ~JournalInstance() = default;
    virtual systemd::Journal create() = 0;

  protected:
    JournalInstance() = default;
};

} // namespace jrn
