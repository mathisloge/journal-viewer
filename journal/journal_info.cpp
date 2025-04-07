// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal_info.hpp"
#include "fmt/base.h"
#include "journal_entry.hpp"
#include "journal_instance.hpp"

namespace jrn
{
JournalInfo::JournalInfo(JournalInstanceHandle handle)
    : journal_{handle->create()}
{
    sd_journal_seek_head(journal_.get());

    sd_journal_query_unique(journal_.get(), kSystemdUnitKey.cbegin());

    const void *data{nullptr};
    size_t length{};
    SD_JOURNAL_FOREACH_UNIQUE(journal_.get(), data, length)
    {

        fmt::println("test  {}", extract_field_data(data, length));
        systemd_units_.emplace(extract_field_data(data, length));
    }
}

const JournalInfo::SystemdUnits &JournalInfo::systemd_units() const noexcept
{
    return systemd_units_;
}
} // namespace jrn
