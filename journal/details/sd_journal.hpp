// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <memory>
#include <systemd/sd-journal.h>
namespace jrn::systemd
{
using Journal = std::unique_ptr<sd_journal, decltype(&sd_journal_close)>;
}
