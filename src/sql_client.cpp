#include "defaults.h"

#include <iostream>
#include <boost/asio.hpp>

#include "client_connection.h"


int main(int c, char *args[])
{
    int port = DEFAULT_PORT; // lucky port
    int o = getopt(c, args, "p:");
    if ('p' == o )
    {
        port = std::stoi(optarg);
    }
    boost::asio::io_service service;

    /* no need it
    asio::signal_set signals{service, SIGINT, SIGTERM};
    signals.async_wait([&](auto, auto) { service.stop(); });
    */

    // do not save pointer of client connection anywere
    // it must be destroyed after service.run() end
    sp_client_connection_c sp_client(new client_connection_c(service, port));
    sp_client->connect();
    service.run();
    return 0;
}
