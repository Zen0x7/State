#include "protocol.hpp"

std::string network::protocol::handle(boost::json::object &request, std::string transaction_id,
                                      std::shared_ptr<state> const &state,
                                      std::shared_ptr<websocket_session> websocket) {
    std::string _action{request.at("action").as_string()};

    boost::json::object missing_required_attributes_message = {
        {"type", "response"},
        {"status", 500},
        {"error", "required_attributes"},
        {"reference", {{"transaction_id", transaction_id}}},
    };

    boost::json::object invalid_values_message = {
        {"type", "response"},
        {"status", 500},
        {"error", "invalid_values"},
        {"reference", {{"transaction_id", transaction_id}}},
    };

    boost::json::object success_message = {
        {"type", "response"},
        {"status", 200},
        {"reference", {{"transaction_id", transaction_id}}},
    };

    boost::json::object unauthorized_message = {
        {"type", "response"},
        {"status", 401},
        {"error", "unauthorized"},
        {"reference", {{"transaction_id", transaction_id}}},
    };

    boost::json::object not_found_message = {
        {"type", "response"},
        {"status", 404},
        {"error", "not_found"},
        {"reference", {{"transaction_id", transaction_id}}},
    };

    if (_action == "registration") {
        if (request.contains("registration_token")) {
            if (request.at("registration_token").is_string()) {
                std::string _registration_token_as_hex{request.at("registration_token").as_string()};
                std::string _registration_token = cipher::decrypt(_registration_token_as_hex);
                if (_registration_token == transaction_id) {
                    websocket->id_ = transaction_id;
                    state->worker_registered(transaction_id, websocket);
                    return serialize(success_message);
                }
                return serialize(unauthorized_message);
            }
            return serialize(invalid_values_message);
        }
        return serialize(missing_required_attributes_message);
    }

    if (_action == "accepted") {
        if (request.contains("address") && request.contains("port")) {
            if (request.at("address").is_string() && request.at("port").is_int64()) {
                std::string _ip{request.at("address").as_string()};
                int64_t _port = request.at("port").as_int64();
                state->user_connected(websocket->id_, transaction_id, _ip, _port);
                return serialize(success_message);
            }
            return serialize(invalid_values_message);
        }
        return serialize(missing_required_attributes_message);
    }

    if (_action == "disconnected") {
        if (request.contains("user_id")) {
            if (request.at("user_id").is_string()) {
                std::string _user_id{request.at("user_id").as_string()};
                if (stamper::is_transaction_id_valid(_user_id)) {
                    if (state->user_is_connected(websocket->id_, _user_id)) {
                        state->user_disconnected(websocket->id_, _user_id);
                        return serialize(success_message);
                    }
                    boost::json::object user_not_found_message = {
                        {"type", "response"},
                        {"status", 500},
                        {"error", "user_not_found"},
                    };
                    return serialize(user_not_found_message);
                }
                boost::json::object invalid_user_id_value_message = {
                    {"type", "response"},
                    {"status", 500},
                    {"error", "invalid_user_id_value"},
                };
                return serialize(invalid_user_id_value_message);
            }
            return serialize(invalid_values_message);
        }
        return serialize(missing_required_attributes_message);
    }

    return serialize(not_found_message);
}
