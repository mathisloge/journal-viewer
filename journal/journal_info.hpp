// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <string>
#include <unordered_set>
#include "details/sd_journal.hpp"
#include "journal_instance_handle.hpp"

namespace jrn
{

/**
 * @brief The JournalInfo contains the information of the journal at global scope, e.g. which units do exist.
 *
 */
class JournalInfo
{
  public:
    using SystemdUnits = std::unordered_set<std::string>;

  public:
    explicit JournalInfo(JournalInstanceHandle handle);
    const SystemdUnits &systemd_units() const noexcept;

  private:
    systemd::Journal journal_;
    SystemdUnits systemd_units_;
};
} // namespace jrn
