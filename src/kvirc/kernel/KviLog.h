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

enum struct LogType {
    Debug,
    Info,
    Error,
};

struct KviLog {
    KviLog(LogType type) : type(type) { };
    ~KviLog() {
        std::string ret = this->build_prefix();
        ret += this->buf.str();

        std::cout << ret << std::endl;
    };

    std::string build_prefix() {
        std::stringstream buf;
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        auto ts = std::put_time(std::localtime(&now_c), "%c %z");

        switch (this->type) {
        case LogType::Debug:
            buf << "[DEBUG";
            break;
        case LogType::Info:
            buf << "[INFO";
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
