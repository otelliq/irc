#include "Client.hpp"
#include "Channel.hpp"
#include "server.hpp"

int main(int ac, char **av)
{
    Server Excalibur ;
    if (ac != 3)
    {
        std::cout << "Usage: " << av[0] << " <port number> <password> " << std::endl;
        return 1;
    }
    try
    {
        // list of signals that interrupt the irc server : 
            signal(SIGINT, Server::Signal_Handler);

        // Launching the Server:
        if (!Excalibur.Port_valid(av[1]))
        {
            std::cout << " Your Port Number is Incorrect or Invalid " << std::endl;
            return 1;
        }
        else if (!*av[2])
        {
            std::cout << " You didnt enter a password " << std::endl;
            return 1;
        }
        else if (std::strlen(av[2]) > 20)
        {
            std::cout <<  "Your Password is More than 20 Character try a shorter version king " << std::endl;
            return 1;
        }
        std::cout << " your server is Launching sir wait a moment ..." << std::endl ;
        Excalibur.Launching_server(std::atoi(av[1]), av[2]);
    }
    catch(const std::exception& e)
    {
        // Excalibur.Close_filedescriptors();
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Saddly we inform u that your Server Closed until another announcement be safe .. " << std::endl;
}
