#ifndef ENTITIES_USER_HPP
#define ENTITIES_USER_HPP

#include <memory>
#include <utility>


namespace entities {
    class user : std::enable_shared_from_this<user> {
    public:
        std::string address_;
        int64_t port_;
        user(std::string address, int64_t port) : address_(std::move(address)), port_(port) {}
    };
}

#endif // ENTITIES_USER_HPP
