// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <memory>
namespace jrn
{
class JournalInstance;
class JournalInstanceHandle
{
  public:
    JournalInstanceHandle() = default;
    JournalInstance *operator->()
    {
        return handle_.get();
    }

    bool valid() const noexcept
    {
        return handle_ != nullptr;
    }

  private:
    JournalInstanceHandle(std::shared_ptr<JournalInstance> handle)
        : handle_{std::move(handle)}
    {}

  private:
    friend JournalInstance;
    std::shared_ptr<JournalInstance> handle_;
};
} // namespace jrn
