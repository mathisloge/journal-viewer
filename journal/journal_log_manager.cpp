#include "journal_log_manager.hpp"
#include "journal_instance.hpp"
namespace jrn
{
JournalLogManager::JournalLogManager(JournalInstanceHandle handle)
    : journal_{handle->create()}
{
    cache_.build_initial_cache(journal_.get());
}
} // namespace jrn
