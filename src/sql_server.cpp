#include <unistd.h> /* for getopt */
#include <string>
#include <iostream>

#include "defaults.h"
#include "service.h"

int main(int c, char *args[])
{
    int port = DEFAULT_PORT; // lucky port
    int o = getopt(c, args, "p:");
    if ('p' == o )
    {
        port = std::stoi(optarg);
    }
    try 
    {
        if ( run_server(port) )
        {
            return EXIT_SUCCESS;
        }
        else
        {
            return EXIT_FAILURE;
        }
    } 
    catch (const std::exception &ex) 
    {
        std::cerr << "Fatal error \"" << ex.what() << "\"." << std::endl;
    } catch (...) 
    {
        std::cerr << "Fatal UNKNOWN error." << std::endl;
    }
    return EXIT_FAILURE;
}