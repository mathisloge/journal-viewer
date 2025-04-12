// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
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

  private:
    asio::strand<asio::any_io_executor> strand_;
    JournalLogManager manager_;
};
} // namespace jrn
