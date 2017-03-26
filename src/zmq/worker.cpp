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
#include <bitcoin/protocol/zmq/worker.hpp>

#include <zmq.h>
#include <functional>
#include <future>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

#define NAME "worker"

// Derive from this abstract worker to implement concrete worker.
worker::worker(thread_priority priority)
  : priority_(priority),
    stopped_(true)
{
}

worker::~worker()
{
    stop();
}

// Restartable after stop and not started on construct.
bool worker::start()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    if (stopped_)
    {
        stopped_ = false;

        // Since one thread per started service is required there is no benefit
        // to using the threadpool. If the threadpool is used there must be
        // sufficient numbers of threads reserved, which is counterproductive.
        ////dispatch_.concurrent(std::bind(&worker::work, this));

        // Create the replier thread and socket and start polling.
        thread_ = std::make_shared<asio::thread>(&worker::work, this);

        // Wait on replier start.
        const auto result = started_.get_future().get();

        // Reset for restartability.
        started_ = std::promise<bool>();
        return result;
    }

    return false;
    ///////////////////////////////////////////////////////////////////////////
}

bool worker::stop()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    if (!stopped_)
    {
        stopped_ = true;

        // Wait on replier stop.
        // This is used instead of thread join in order to capture result.
        const auto result = finished_.get_future().get();

        // Reset for restartability.
        finished_ = std::promise<bool>();
        return result;
    }

    return true;
    ///////////////////////////////////////////////////////////////////////////
}

// Utilities.
//-----------------------------------------------------------------------------

// Call from work to detect an explicit stop.
bool worker::stopped()
{
    return stopped_;
}

// Call from work when started (connected/bound) or failed to do so.
bool worker::started(bool result)
{
    started_.set_value(result);

    if (result)
        set_priority(priority_);
    else
        finished(true);

    return result;
}

// Call from work when finished working, do not call if started was called.
bool worker::finished(bool result)
{
    finished_.set_value(result);
    return result;
}

// Call from work to forward a message from one socket to another.
bool worker::forward(socket& from, socket& to)
{
    message packet;
    return !from.receive(packet) && !to.send(packet);
}

// Call from work to establish a proxy between two sockets.
void worker::relay(socket& left, socket& right)
{
    // Blocks until the context is terminated, always returns -1.
    zmq_proxy_steerable(left.self(), right.self(), nullptr, nullptr);

    // Equivalent implementation:
    ////zmq::poller poller;
    ////poller.add(left);
    ////poller.add(right);
    ////
    ////while (!poller.terminated())
    ////{
    ////    const auto signaled = poller.wait();
    ////
    ////    if (signaled.contains(left.id()))
    ////        forward(left, right);
    ////
    ////    if (signaled.contains(right.id()))
    ////        forward(right, left);
    ////}
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin
