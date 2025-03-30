#pragma once
#include <journal_info.hpp>
#include <journal_instance_handle.hpp>
#include <journal_log_manager.hpp>

namespace jrn
{
struct JournalLogWindow
{
  public:
    explicit JournalLogWindow(std::string title, JournalInstanceHandle handle, const JournalInfo &info);
    void draw();

  private:
    void draw_priority_filter(std::string_view title, Priority priority);

  private:
    std::string title_;
    bool open_{true};
    const JournalInfo &info_;
    JournalLogManager manager_;
};
} // namespace jrn
