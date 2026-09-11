#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>

#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket&& socket, ssl::context& ctx)
    : ws{std::move(socket), ctx}, timer{ws.get_executor()}
    {
    }

    void run()
    {
        net::dispatch(
            ws.get_executor(),
            beast::bind_front_handler(&Session::onRun, shared_from_this())
        );
    }

    static inline auto extractTicker(std::string const target) -> std::string
    {
        std::string const prefix{"/marketdata/"};

        if (target.rfind(prefix, 0) == 0 && target.length() > prefix.length())
        {
            return target.substr(prefix.length());
        }

        return "";
    }

private:
    void onRun()
    {
        beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
        ws.next_layer().async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(&Session::onHandshakeSSL, shared_from_this())
        );
    }

    void onHandshakeSSL(beast::error_code const ec)
    {
        if (ec)
        {
            std::cerr << "SSL handshake failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "SSL handshake succeeded\n";

        beast::get_lowest_layer(ws).expires_never();
        ws.set_option(
            websocket::stream_base::timeout::suggested(beast::role_type::server)
        );
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server, "MarketDataServer");
            })
        );

        ws.async_accept(
            beast::bind_front_handler(&Session::onHandshakeWS, shared_from_this())
        );
    }

    void onHandshakeWS(beast::error_code const ec)
    {
        if (ec)
        {
            std::cerr << "WS Handshake failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "WS Handshake succeeded\n";
        schedulePublishing();
    }

    void generateMessages()
    {
        auto const ticker = "BTCUSDT";

        auto addMessage = [&](std::string side, double price, double quantity)
        {
            nlohmann::json const message = {
                {"ticker", ticker},
                {"side", side},
                {"price", price},
                {"quantity", quantity}
            };

            messages.push(message);
        };

        addMessage("ask", 78000.75, 3.0000);
        addMessage("ask", 78000.60, 2.0000);
        addMessage("ask", 78000.45, 1.0000);
        addMessage("bid", 78000.30, 1.0000);
        addMessage("bid", 78000.15, 2.0000);
        addMessage("bid", 78000.00, 3.0000);
    }

    void doPublish()
    {
        if (messages.empty())
        {
            std::cout << "No more messages in the queue\n";
            schedulePublishing();
            return;
        }

        auto const message = messages.front();
        ws.text(true);
        ws.async_write(
            net::buffer(message.dump()),
            beast::bind_front_handler(&Session::onPublish, shared_from_this())
        );
        messages.pop();
    }

    void onPublish(beast::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
        {
            std::cerr << "Publish failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Publish succeeded (" << bytes_transferred << " bytes transferred)\n";
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        doPublish();
    }

    void schedulePublishing()
    {
        timer.expires_after(std::chrono::milliseconds(1000));
        timer.async_wait(
            beast::bind_front_handler(&Session::onScheduleEvent, shared_from_this()));
    }

    void cancelPublishing()
    {
        beast::error_code ec;
        timer.cancel(ec);

        if (ec)
        {
            std::cerr << "Stop timer failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Stop timer succeeded\n";
    }

    void onScheduleEvent(beast::error_code const ec)
    {
        if (ec)
        {
            std::cerr << "Schedule event failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Schedule event succeeded\n";
        generateMessages();
        doPublish();
    }

    void doClose()
    {
        std::cout << "Closing...\n";
        ws.async_close(
            websocket::close_code::normal,
            beast::bind_front_handler(&Session::onClose, shared_from_this())
        );
    }

    void onClose(beast::error_code const ec)
    {
        cancelPublishing();

        if (ec)
        {
            std::cerr << "Close failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Close succeeded\n";
    }

    websocket::stream<ssl::stream<beast::tcp_stream>> ws;
    beast::flat_buffer buffer;
    net::steady_timer timer;
    std::queue<nlohmann::json> messages;
};

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener(net::io_context& ioc, ssl::context& ctx, tcp::endpoint const endpoint)
    : ioctx{ioc}, sslctx{ctx}, acceptor{net::make_strand(ioc)}
    {
        beast::error_code ec;

        acceptor.open(endpoint.protocol(), ec);

        if (ec)
        {
            std::cerr << "Socket creation failed: " << ec.message() << "\n";
            return;
        }

        acceptor.set_option(net::socket_base::reuse_address(true), ec);

        if (ec)
        {
            std::cerr << "Set option failed: " << ec.message() << "\n";
            return;
        }

        acceptor.bind(endpoint, ec);

        if (ec)
        {
            std::cerr << "Bind failed: " << ec.message() << "\n";
            return;
        }

        acceptor.listen(net::socket_base::max_listen_connections, ec);

        if (ec)
        {
            std::cerr << "Listen failed: " << ec.message() << "\n";
            return;
        }

        std::cout << "Listen OK\n";
    }

    void run()
    {
        doAccept();
    }

private:
    void doAccept()
    {
        acceptor.async_accept(
            net::make_strand(ioctx),
            beast::bind_front_handler(&Listener::onAccept, shared_from_this())
        );
    }

    void onAccept(beast::error_code const ec, tcp::socket socket)
    {
        if (!ec)
        {
            std::make_shared<Session>(std::move(socket), sslctx)->run();
        }

        std::cout << "Accepting...\n";
        doAccept();
    }

    net::io_context& ioctx;
    ssl::context& sslctx;
    tcp::acceptor acceptor;
};

int main()
{
    auto const address{net::ip::make_address("127.0.0.1")};
    unsigned short const port{8080};
    net::io_context ioc;
    ssl::context ctx{ssl::context::tls_server};
    ctx.use_certificate_chain_file("server.crt");
    ctx.use_private_key_file("server.key", ssl::context::pem);
    std::make_shared<Listener>(ioc, ctx, tcp::endpoint{address, port})->run();
    std::cout << "Listening on: " << address << "\n";
    ioc.run();
    return EXIT_SUCCESS;
}
