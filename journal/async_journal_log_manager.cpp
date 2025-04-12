// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "async_journal_log_manager.hpp"
#include <asio/io_context.hpp>

namespace jrn
{
AsyncJournalLogManager::AsyncJournalLogManager(asio::io_context &io_ctx, JournalInstanceHandle handle)
    : strand_{io_ctx.get_executor()}
    , manager_{handle}
{}
} // namespace jrn
