#ifndef NETWORK_STAMPER_HPP
#define NETWORK_STAMPER_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <iostream>

namespace network {
    class stamper {
    public:
        static bool is_transaction_id_valid(std::string const & input) {
            using namespace boost::uuids;
            try {
                auto result = string_generator()(input);
                return result.version() != uuid::version_unknown;
            } catch(...) {
                return false;
            }
        }
    };
}

#endif // NETWORK_STAMPER_HPP
