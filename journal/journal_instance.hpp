// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <filesystem>
#include "details/sd_journal.hpp"
#include "journal_instance_handle.hpp"
#include "result.hpp"

namespace jrn
{
class JournalInstance
{
  public:
    static Result<JournalInstanceHandle> from_directory(std::filesystem::path directory);

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
