/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_PROTOCOL_SETTINGS_HPP
#define LIBBITCOIN_PROTOCOL_SETTINGS_HPP

#include <cstdint>
////#include <string>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {

/// Most values are capped at max_int32.
/// Common protocol configuration settings, properties not thread safe.
class BCP_API settings
{
public:
    settings();

    // ZMQ_SNDHWM (0 unlimited)
    uint32_t send_high_water;

    // ZMQ_RCVHWM (0 unlimited)
    uint32_t receive_high_water;

    /// ZMQ_MAXMSGSIZE (0 unlimited)
    uint32_t message_size_limit;

    /// ZMQ_HANDSHAKE_IVL (0 unlimited)
    uint32_t handshake_seconds;

    /// ZMQ_HEARTBEAT_IVL (0 disabled)
    uint32_t heartbeat_seconds;

    /// ZMQ_HEARTBEAT_TTL and ZMQ_HEARTBEAT_TIMEOUT (0 disabled)
    uint32_t inactivity_seconds;

    // ZMQ_SNDTIMEO (0 unlimited)
    uint32_t send_milliseconds;

    // Client (connector) setting.
    // ZMQ_RECONNECT_IVL and ZMQ_RECONNECT_IVL_MAX (0 disabled)
    uint32_t reconnect_seconds;
};

} // namespace blockchain
} // namespace protocol

#endif
