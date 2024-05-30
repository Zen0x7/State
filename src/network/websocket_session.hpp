#ifndef NETWORK_WEBSOCKET_SESSION_HPP
#define NETWORK_WEBSOCKET_SESSION_HPP

#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../state.hpp"
#include "../protocol.hpp"

#include "fail.hpp"
#include "stamper.hpp"

namespace network {
    class websocket_session : public std::enable_shared_from_this<websocket_session> {
        boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
        boost::beast::flat_buffer buffer_;
        std::shared_ptr<state> state_;

    public:
        // Take ownership of the socket
        explicit
        websocket_session(boost::asio::ip::tcp::socket &&socket, std::shared_ptr<state> const &state)
            : ws_(std::move(socket)), state_(state) {
        }

        // Start the asynchronous accept operation
        template<class Body, class Allocator>
        void
        do_accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator> > req) {
            // Set suggested timeout settings for the websocket
            ws_.set_option(
                boost::beast::websocket::stream_base::timeout::suggested(
                    boost::beast::role_type::server));

            // Set a decorator to change the Server of the handshake
            ws_.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type &res) {
                    res.set(boost::beast::http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " advanced-server");
                }));

            // Accept the websocket handshake
            ws_.async_accept(
                req,
                boost::beast::bind_front_handler(
                    &websocket_session::on_accept,
                    shared_from_this()));
        }

    private:
        void
        on_accept(boost::beast::error_code ec) {
            if (ec)
                return fail(ec, "accept");

            // Read a message
            do_read();
        }

        void
        do_read() {
            // Read a message into our buffer
            ws_.async_read(
                buffer_,
                boost::beast::bind_front_handler(
                    &websocket_session::on_read,
                    shared_from_this()));
        }

        void
        on_read(
            boost::beast::error_code ec,
            std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            // This indicates that the websocket_session was closed
            if (ec == boost::beast::websocket::error::closed)
                return;

            if (ec)
                fail(ec, "read");

            // Echo the message
            ws_.text(ws_.got_text());

            boost::system::error_code _parser_ec;

            std::string _data = boost::beast::buffers_to_string(buffer_.data());

            std::cout << "Received: " << _data << std::endl;

            auto _message = boost::json::parse(_data, _parser_ec);

            if (!_parser_ec && _message.is_object()) {
                if (_message.as_object().contains("action")) {
                    if (_message.as_object().at("action").is_string()) {
                        if (_message.as_object().contains("transaction_id")) {
                            if (_message.as_object().at("transaction_id").is_string()) {
                                std::string _transaction_id{_message.as_object().at("transaction_id").as_string()};
                                if (stamper::is_transaction_id_valid(_transaction_id)) {
                                    std::string response = protocol::handle(_message.as_object(), _transaction_id, state_);
                                    ws_.async_write(
                                        boost::asio::buffer(response),
                                        boost::beast::bind_front_handler(
                                            &websocket_session::on_write,
                                            shared_from_this()));
                                } else {
                                    boost::json::object invalid_transaction_id_value_message = {
                                        {"type", "response"},
                                        {"status", 500},
                                        {"error", "invalid_transaction_id_value"},
                                        {"message", "value of transaction_id attribute in message must be a uuidv4"}
                                    };
                                    ws_.async_write(
                                        boost::asio::buffer(
                                            boost::json::serialize(invalid_transaction_id_value_message)),
                                        boost::beast::bind_front_handler(
                                            &websocket_session::on_write,
                                            shared_from_this()));
                                }
                            } else {
                                boost::json::object invalid_transaction_id_attribute_type_message = {
                                    {"type", "response"},
                                    {"status", 500},
                                    {"error", "invalid_transaction_id_attribute_type"},
                                    {"message", "attribute transaction_id in message must be a string"}
                                };
                                ws_.async_write(
                                    boost::asio::buffer(
                                        boost::json::serialize(invalid_transaction_id_attribute_type_message)),
                                    boost::beast::bind_front_handler(
                                        &websocket_session::on_write,
                                        shared_from_this()));
                            }
                        } else {
                            boost::json::object missing_transaction_id_attribute_message = {
                                {"type", "response"},
                                {"status", 500},
                                {"error", "missing_transaction_id_attribute"},
                                {"message", "message must include transaction_id attribute"}
                            };
                            ws_.async_write(
                                boost::asio::buffer(boost::json::serialize(missing_transaction_id_attribute_message)),
                                boost::beast::bind_front_handler(
                                    &websocket_session::on_write,
                                    shared_from_this()));
                        }
                    } else {
                        boost::json::object invalid_action_attribute_type_message = {
                            {"type", "response"},
                            {"status", 500},
                            {"error", "invalid_action_attribute_type"},
                            {"message", "attribute action in message must be a string"}
                        };
                        ws_.async_write(
                            boost::asio::buffer(boost::json::serialize(invalid_action_attribute_type_message)),
                            boost::beast::bind_front_handler(
                                &websocket_session::on_write,
                                shared_from_this()));
                    }
                } else {
                    boost::json::object missing_action_attribute_message = {
                        {"type", "response"},
                        {"status", 500},
                        {"error", "missing_action_attribute"},
                        {"message", "message must include action attribute"}
                    };
                    ws_.async_write(
                        boost::asio::buffer(boost::json::serialize(missing_action_attribute_message)),
                        boost::beast::bind_front_handler(
                            &websocket_session::on_write,
                            shared_from_this()));
                }
            } else {
                boost::json::object invalid_object_message = {
                    {"type", "response"},
                    {"status", 500},
                    {"error", "invalid_json"},
                    {"message", "message must be a valid JSON object"}
                };
                ws_.async_write(
                    boost::asio::buffer(boost::json::serialize(invalid_object_message)),
                    boost::beast::bind_front_handler(
                        &websocket_session::on_write,
                        shared_from_this()));
            }
        }

        void
        on_write(
            boost::beast::error_code ec,
            std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);

            if (ec)
                return fail(ec, "write");

            // Clear the buffer
            buffer_.consume(buffer_.size());

            // Do another read
            do_read();
        }
    };
};


#endif // NETWORK_WEBSOCKET_SESSION_HPP
