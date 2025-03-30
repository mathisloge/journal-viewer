#pragma once
#include <journal_instance_handle.hpp>
#include <journal_log_manager.hpp>

namespace jrn
{
struct JournalLogWindow
{
  public:
    explicit JournalLogWindow(std::string title, JournalInstanceHandle handle);
    void draw();

  private:
    std::string title_;
    bool open_{true};
    JournalLogManager manager_;
};
} // namespace jrn
