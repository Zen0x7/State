#include "protocol.hpp"

std::string network::protocol::handle(boost::json::object &request, std::string transaction_id,
                                      std::shared_ptr<state> const &state,
                                      std::shared_ptr<websocket_session> websocket) {
    std::string _action{request.at("action").as_string()};

    if (_action == "join") {
        if (request.contains("registration_token")) {
            if (request.at("registration_token").is_string()) {
                std::string _registration_token_as_hex{request.at("registration_token").as_string()};
                std::string _registration_token = cipher::decrypt(_registration_token_as_hex);

                if (_registration_token == transaction_id) {
                    websocket->id_ = std::make_shared<std::string>(transaction_id);

                    boost::json::object join_accepted_message = {
                        {"type", "response"},
                        {"status", 202},
                        {"reference", {{"transaction_id", transaction_id}}},
                    };
                    return serialize(join_accepted_message);
                }

                boost::json::object unaccepted_registration_token_message = {
                    {"type", "response"},
                    {"status", 401},
                    {"error", "unaccepted_registration_token_attribute_type"},
                    {"message", "attribute registration_token isn't accepted"},
                    {"reference", {{"transaction_id", transaction_id}}},
                };
                return serialize(unaccepted_registration_token_message);
            }

            boost::json::object invalid_registration_token_message = {
                {"type", "response"},
                {"status", 500},
                {"error", "invalid_registration_token_attribute_type"},
                {"message", "attribute registration_token in message must be a string"},
                {"reference", {{"transaction_id", transaction_id}}},
            };
            return serialize(invalid_registration_token_message);
        }

        boost::json::object missing_registration_token_message = {
            {"type", "response"},
            {"status", 500},
            {"error", "missing_registration_token_attribute"},
            {"message", "message must include registration_token attribute"},
            {"reference", {{"transaction_id", transaction_id}}},
        };
        return serialize(missing_registration_token_message);
    }

    if (_action == "user_accepted") {
        boost::json::object user_accepted_message = {
            {"type", "response"},
            {"status", 202},
            {"reference", {{"transaction_id", transaction_id}}},
        };
        return serialize(user_accepted_message);
    }

    boost::json::object generic_response_message = {
        {"type", "response"},
        {"status", 200},
        {"reference", {{"transaction_id", transaction_id}}},
    };
    return serialize(generic_response_message);
}
