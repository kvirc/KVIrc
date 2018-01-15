//=============================================================================
//
//   File : KviLog.h
//   Creation date : Sun 14 2018 18:09:06 by Matt Ullman
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2018 Matt Ullman (staticfox at staticfox dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#ifndef KVI_LOG_H
#define KVI_LOG_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

#include <QString>

enum struct LogType {
    Debug,
    Info,
    Error,
    Fatal,
};

struct KviLog {
    KviLog(LogType type) : type(type) { };
    ~KviLog() {
        std::string ret = this->build_prefix();
        ret += this->buf.str();

        std::cout << ret << std::endl;
    };

    // localtime, put_time, systemtime_now courtesy of:
    // https://kjellkod.wordpress.com/2013/01/22/exploring-c11-part-2-localtime-and-time-again/
    // https://sites.google.com/site/kjellhedstrom2//g2log-efficient-background-io-processign-with-c11
    tm localtime(const std::time_t& time) {
        std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
        localtime_s(&tm_snapshot, &time);
#else
        localtime_r(&time, &tm_snapshot); // POSIX
#endif
      return tm_snapshot;
    }

    // To simplify things the return value is just a string. I.e. by design!
    std::string put_time(const std::tm* date_time, const char* c_time_format) {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
        std::ostringstream oss;

        // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
        // ref. C++11 standard: ISO/IEC 14882:2011, § 27.7.1,
        oss << std::put_time(const_cast<std::tm*>(date_time), c_time_format);
        return oss.str();

#else    // LINUX
        const size_t size = 1024;
        char buffer[size];
        auto success = std::strftime(buffer, size, c_time_format, date_time);

        if (0 == success)
            return c_time_format;

        return buffer;
#endif
    }

    // extracting std::time_t from std:chrono for "now"
    std::time_t systemtime_now() {
        auto system_now = std::chrono::system_clock::now();
        return std::chrono::system_clock::to_time_t(system_now);
    }

    std::string build_prefix() {
        std::stringstream buf;

        std::tm t1 = localtime(systemtime_now());
        auto ts = put_time(&t1, "%c %z");

        switch (this->type) {
        case LogType::Debug:
            buf << "[DEBUG";
            break;
        case LogType::Info:
            buf << "[INFO";
            break;
        case LogType::Fatal:
            buf << "[FATAL";
            break;
        default:
            buf << "[UNKNOWN";
            break;
        }

        buf << "/(" << ts << ")] ";
        return buf.str();
    }

    template<typename T>
    KviLog& operator<<(T val) {
        this->buf << val;
        return *this;
    }

    template<>
    KviLog& operator<<(QString val) {
        this->buf << val.toStdString();
        return *this;
    }

    LogType type;
    std::stringstream buf;
};

#endif
