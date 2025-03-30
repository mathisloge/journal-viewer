#include "journal_info.hpp"
#include "journal_entry.hpp"
#include "journal_instance.hpp"

namespace jrn
{
JournalInfo::JournalInfo(JournalInstanceHandle handle)
    : journal_{handle->create()}
{
    sd_journal_seek_head(journal_.get());

    while (sd_journal_next(journal_.get()) > 0)
    {
        systemd_units_.emplace(get_systemd_unit(journal_.get()));
    }
}

const JournalInfo::SystemdUnits &JournalInfo::systemd_units() const noexcept
{
    return systemd_units_;
}
} // namespace jrn
