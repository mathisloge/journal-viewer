// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <quill/LogMacros.h>
#include <quill/Logger.h>

namespace jrn
{
void setup_logger();
quill::Logger *create_logger(std::string_view logger_name);
} // namespace jrn

#define DEFINE_LOGGER(name)                                                                                            \
    namespace                                                                                                          \
    {                                                                                                                  \
    auto l_##name()                                                                                                    \
    {                                                                                                                  \
        static const auto &&name_logger = ::jrn::create_logger(#name);                                                 \
        return name_logger;                                                                                            \
    }                                                                                                                  \
    }
