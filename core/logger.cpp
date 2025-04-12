// SPDX-FileCopyrightText: 2025 Mathis Logemann <mathisloge@tuta.io>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journal/logger.hpp"
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/FileSink.h>

namespace jrn
{
namespace
{
constexpr std::string kDefaultSink = "defaul_sink";
}
void setup_logger()
{
    quill::Backend::start();
    quill::Frontend::preallocate();
    auto default_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(kDefaultSink);
    auto *root_logger = quill::Frontend::create_or_get_logger("root", std::move(default_sink));
    root_logger->set_log_level(quill::LogLevel::Debug);
    QUILL_LOG_INFO(root_logger, "logging finished setup");
}

quill::Logger *create_logger(std::string_view logger_name)
{
    try
    {
        auto default_sink = quill::Frontend::get_sink(kDefaultSink);
        auto &&logger = quill::Frontend::create_or_get_logger(std::string{logger_name}, std::move(default_sink));
        logger->set_log_level(quill::LogLevel::Debug);

        return logger;
    }
    catch (const quill::QuillError &ex)
    {
        auto default_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(kDefaultSink);
        auto &&logger = quill::Frontend::create_or_get_logger(std::string{logger_name}, std::move(default_sink));
        logger->set_log_level(quill::LogLevel::Debug);

        QUILL_LOG_CRITICAL(logger, "Did you forget to call jrn::setup_logger()? Error: {}", ex.what());

        return logger;
    }
}
} // namespace jrn
