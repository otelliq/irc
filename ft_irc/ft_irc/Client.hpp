#pragma once 
#include <iostream>
#include <sys/socket.h> // contains definitions of function and structures , socket(), bind(), listen(), and accept(). Defines the sockaddr structure, which is a generic structure for all socket addresses.
#include <cstring> 
#include <unistd.h>
#include <string>
#include <vector>
#include <netinet/in.h>  // Contains definitions for Internet Protocol family , Defines the sockaddr_in structure, used for IPv4 addresses. , Contains macros like INADDR_ANY and functions like htons() .
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <algorithm>
#include <stdlib.h>
#include "Channel.hpp"
#include "server.hpp"
#include "Parcing_data.hpp"


class Channel;
class Server;

class Client
{
    private:
        std::string  nickname;
        std::string  username;
        std::string  hostname;
        std::string  servername;
        std::string  realname;
        bool has_joined;

        int clientfd;
        std::string clientIP; 
        bool Operator_status;
        bool registration_status;
        bool loged_status;

    public:
    
        // Canonical form:
            Client();
            Client(const Client& cli);
            Client &operator=(Client const &src);
            ~Client();   
        
        // Setters
            void setfd(int fd);
            void setnickname( std::string & nickName);
            void setusername(std::string  userName);
            void sethostname(std::string & hostname);
            void setservername(std::string & servername);
            void setrealname(std::string & relaname);
            void setIPaddress(std::string Ipadd);
            void setregistred(bool reg);
            void setoperatorstatus(bool oper);
            void setlogedstatus(bool log);
        
        // Getters
            std::string getnickname();
            std::string getusername();
            std::string gethostname();
            std::string getservername();
            std::string getrealname();
            std::string getIPaddress();
            std::string getPrefix();
            bool getregistred();
            bool getoperatorstatus();
            bool getlogedstatus();
            int get_clientfd();
            void client_data();


            void sendError(Client *client, const std::string& errorCode, const std::string& channel, const std::string& message);

};
