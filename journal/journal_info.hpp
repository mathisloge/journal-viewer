#pragma once
#include <string>
#include <unordered_set>

namespace jrn
{

/**
 * @brief The JournalInfo contains the information of the journal at global scope, e.g. which units do exist.
 *
 */
class JournalInfo
{
    std::unordered_set<std::string> units_;
};
} // namespace jrn
