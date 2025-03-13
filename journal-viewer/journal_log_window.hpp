#pragma once
#include <journal_log_manager.hpp>

namespace jrn
{
struct JournalLogWindow
{
  public:
    JournalLogWindow(std::string title, const std::filesystem::path &file_or_directory);
    void draw();

  private:
    std::string title_;
    bool open_{true};
    JournalLogManager manager_;
};
} // namespace jrn
