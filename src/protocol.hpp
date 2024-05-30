#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

class protocol {
public:
    static std::string handle(
        boost::json::object &request,
        std::string transaction_id,
        std::shared_ptr<state> const &state) {
        std::string _action{request.at("action").as_string()};

        if (_action == "join") {
            boost::json::object join_response_message = {
                {"type", "response"},
                {"status", 202},
                {"reference", {{"transaction_id", transaction_id}}},
            };
            return boost::json::serialize(join_response_message);
        }

        if (_action == "user_accepted") {
            boost::json::object user_accepted_response_message = {
                {"type", "response"},
                {"status", 202},
                {"reference", {{"transaction_id", transaction_id}}},
            };
            return boost::json::serialize(user_accepted_response_message);
        }

        boost::json::object generic_response_message = {
            {"type", "response"},
            {"status", 200},
            {"reference", {{"transaction_id", transaction_id}}},
        };
        return boost::json::serialize(generic_response_message);
    }
};

#endif // PROTOCOL_HPP
