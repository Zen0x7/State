#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <vector>

#include "entities/worker.hpp"

class state {
    std::unordered_map<std::string, std::shared_ptr<entities::worker>> workers_;

public:
    void push_back(std::string const & transaction_id) {
        workers_.insert({ transaction_id, {
        }});
    }

    void remove(std::string const & transaction_id) {

    }

};

#endif //STATE_HPP
