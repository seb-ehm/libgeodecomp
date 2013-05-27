#ifndef LIBGEODECOMP_MISC_COMMANDSERVER_H
#define LIBGEODECOMP_MISC_COMMANDSERVER_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <cerrno>
#include <stdexcept>
#include <map>
#include <libgeodecomp/misc/stringops.h>

namespace LibGeoDecomp {

namespace RemoteSteererHelpers {

using boost::asio::ip::tcp;

/*
 * a server, which can be reached by tcp(nc, telnet, ...)
 * executes methods, which are bind to a command
 */
// fixme: needs test
class CommandServer
{
public:
    typedef boost::shared_ptr<tcp::socket> SocketPtr;
    typedef std::vector<std::string> StringVec;
    // fixme: no void pointers!
    typedef std::map<std::string, void(*)(StringVec, CommandServer*, void*)> FunctionMap;

    CommandServer(int port, const FunctionMap& commandMap, void *userData) :
        port(port),
        commandMap(commandMap),
        userData(userData),
        serverThread(&CommandServer::runServer, this)
    {}

    ~CommandServer()
    {
        // fixme:
        // socket.close();
        // serverThread.join();
    }

    // fixme: only needed by help function, get rid of this
    FunctionMap getMap()
    {
        return commandMap;
    }

    size_t sendMessage(const std::string& message)
    {
        boost::system::error_code errorCode;
        size_t bytes = boost::asio::write(
            *socket,
            boost::asio::buffer(message),
            boost::asio::transfer_all(),
            errorCode);

        if (errorCode) {
            printError(errorCode);
        }
        return bytes;
    }

private:
    int port;
    SocketPtr socket;
    FunctionMap commandMap;
    // fixme: no void pointers
    void *userData;
    boost::thread serverThread;

    void runSession ()
    {
        for (;;) {
            boost::array<char, 1024> buf;
            boost::system::error_code errorCode;
            size_t length = socket->read_some(boost::asio::buffer(buf), errorCode);

            if (length > 0) {
                std::string input(buf.data(), length);
                handleInput(input);
            }

            if (errorCode == boost::asio::error::eof) {
                std::cout << "client closed connection" << std::endl;
                return; // Connection closed cleanly by peer.
            }

            if (errorCode) {
                printError(errorCode);
                return;
            }
        }
    }

    void handleInput(const std::string& input)
    {
        StringOps::StringVec lines = StringOps::tokenize(input, "\n");
        for (StringOps::StringVec::iterator iter = lines.begin();
             iter != lines.end();
             ++iter) {
            StringOps::StringVec parameters = StringOps::tokenize(*iter, " ");

            if (parameters.size() > 0) {
                sendMessage("no command\n");
            }

            FunctionMap::iterator it = commandMap.find(parameters[0]);
            if (it != commandMap.end()) {
                commandMap[parameters[0]](parameters, this, userData);
            } else {
                std::string message = "command not found: " + parameters[0] + "\n";
                message += "try \"help\"\n";
                sendMessage(message);
            }
        }
    }

    int runServer()
    {
        try {
            boost::asio::io_service ioService;
            tcp::acceptor acc(ioService, tcp::endpoint(tcp::v4(), port));
            boost::system::error_code ec;

            std::cout << "Commandserver started" << std::endl;

            for (;;) {
                socket = SocketPtr(new tcp::socket(ioService));
                acc.accept(*socket, ec);
                if (ec) {
                    printError(ec);
                } else {
                    std::cout << "client connected" << std::endl;
                    runSession();
                }
            }
        }
        catch (std::exception& e) {
            // fixme: use logger here
            std::cerr << "Exception: " << e.what() << "\n";
            return 1;
        }
    }

    // fixme: kill this
    static void printError(boost::system::error_code& ec)
    {
        std::cerr << "error: " << ec.message() << std::endl;
    }

};


}

}

#endif
