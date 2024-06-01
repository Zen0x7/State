#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <vector>

#include "network/websocket_session.hpp"
#include "entities/worker.hpp"

class state {
    std::unordered_map<std::string, std::shared_ptr<entities::worker> > workers_;

public:
    void add_user_to_worker(std::string const & worker_id, std::string & user_id, std::string & address, int64_t port) {
        workers_[worker_id]->add_user(user_id, address, port);
    }

    void add_worker(std::string const &transaction_id, std::shared_ptr<network::websocket_session> &websocket) {
        workers_.insert({transaction_id, std::make_shared<entities::worker>(websocket)});
    }

    void remove(std::string &websocket_id) {
        workers_.erase(websocket_id);
    }

    boost::json::array get_workers() {
        boost::json::array wrapper;

        for (auto element: workers_) {
            wrapper.emplace_back(boost::json::object({
                {"id", element.first},
                {"address", element.second->get_address()},
                {"port", element.second->get_port()}
            }));
        }

        return wrapper;
    }

    boost::json::array get_users_of_worker(std::string & worker_id) {
        return workers_[worker_id]->get_users();
    }

    boost::json::object get_worker(std::string &id) {
        if (workers_.contains(id)) {
            auto item = workers_[id];
            return {
                {
                    "data", {
                        {"id", id },
                        { "users", get_users_of_worker(id) }
                    }
                }
            };
        }

        return {{"status", "not found"}};
    }
};

#endif //STATE_HPP
