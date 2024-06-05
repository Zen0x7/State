#pragma once

#ifndef NETWORK_PROTOCOL_HPP
#define NETWORK_PROTOCOL_HPP

#include "../cipher.hpp"

#include <boost/json.hpp>

#include "../state.hpp"
#include "websocket_session.hpp"

class state;

namespace network {
    class protocol {
    public:
        static boost::json::object handle(
            boost::json::object &request,
            std::string transaction_id,
            std::shared_ptr<state> const &state,
            std::shared_ptr<websocket_session> websocket);
    };
}

#endif // NETWORK_PROTOCOL_HPP
