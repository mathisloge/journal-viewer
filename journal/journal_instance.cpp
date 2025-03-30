#include "journal_instance.hpp"
#include <format>

namespace jrn
{
class JournalDirectoryHandle : public JournalInstance
{
  public:
    explicit JournalDirectoryHandle(std::filesystem::path file_or_directory)
        : file_or_directory_{std::move(file_or_directory)}
    {}

    systemd::Journal create() override
    {
        systemd::Journal journal{nullptr, sd_journal_close};
        if (std::filesystem::is_directory(file_or_directory_))
        {
            sd_journal *tmp{nullptr};
            int r = sd_journal_open_directory(&tmp, file_or_directory_.c_str(), 0);
            // todo handle r error
            journal.reset(std::move(tmp));
        }
        return journal;
    }

  private:
    std::filesystem::path file_or_directory_;
};

Result<JournalInstanceHandle> JournalInstance::from_directory(std::filesystem::path directory)
{
    if (not std::filesystem::is_directory(directory))
    {
        return make_error_result(std::format("{} is not a directory.", directory.c_str()));
    }
    return JournalInstanceHandle{std::make_shared<JournalDirectoryHandle>(std::move(directory))};
}
} // namespace jrn
