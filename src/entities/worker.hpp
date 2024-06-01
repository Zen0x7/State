#ifndef ENTITIES_WORKER_HPP
#define ENTITIES_WORKER_HPP
#include <memory>

#include "../network/websocket_session.hpp"

#include "user.hpp"

namespace entities {
    class worker : std::enable_shared_from_this<worker> {
        std::shared_ptr<network::websocket_session> websocket_;
        std::unordered_map<std::string, std::shared_ptr<user>> users_;
    public:
        explicit worker(std::shared_ptr<network::websocket_session> websocket) : websocket_(std::move(websocket)) {}
        std::string get_address() { return websocket_->remote_endpoint().address().to_string(); }
        uint_least16_t get_port() { return websocket_->remote_endpoint().port(); }
        void add_user(std::string & id, std::string & address, int64_t port) {
            users_.insert({id, std::make_shared<user>(address, port)});
        }

        boost::json::array get_users() {
            boost::json::array wrapper;

            for (auto element: users_) {
                wrapper.emplace_back(boost::json::object({
                    {"id", element.first},
                    {"address", (*element.second).address_ },
                    {"port", (*element.second).port_ }
                }));
            }

            return wrapper;
        }
    };
}

#endif // ENTITIES_WORKER_HPP
