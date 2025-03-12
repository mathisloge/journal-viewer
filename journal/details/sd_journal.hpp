#pragma once
#include <memory>
#include <systemd/sd-journal.h>
namespace jrn::systemd
{
using Journal = std::unique_ptr<sd_journal, decltype(&sd_journal_close)>;
}
