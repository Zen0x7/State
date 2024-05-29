#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <vector>

#include "entities/worker.hpp"

class state {
    std::vector<std::shared_ptr<entities::worker>> workers_;
};

#endif //STATE_HPP
