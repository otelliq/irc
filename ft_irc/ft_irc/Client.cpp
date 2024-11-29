#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#include "Client.hpp"

// Canonical Form:

Client::~Client()
{}

Client::Client()
{
    this->nickname = "";
    this->username = "";
    this->hostname = "";
    this->servername = "";
    this->realname = "";
    this->clientIP = "";
    this->clientfd = -1;
    this->Operator_status = 0;
    this->loged_status = 0;
    this->registration_status = 0;
    this->has_joined = 0;
}

Client::Client(Client const &cli)
{
    *this = cli;
}
Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->nickname = src.nickname;
        this->username = src.username;
        this->hostname = src.hostname;
        this->realname = src.realname;
        this->servername = src.servername;
        this->clientfd = src.clientfd;
        this->clientIP = src.clientIP;
        this->Operator_status = src.Operator_status;
        this->loged_status = src.loged_status;
        this->has_joined = src.has_joined;
        this->registration_status = src.registration_status;
    }
    return *this;
}

// Client getters:

std::string Client::getnickname()
{
    return this->nickname;  
}
std::string Client::getusername()
{
    return this->username;
}

std::string Client::gethostname()
{
    return this->hostname;
}
std::string Client::getservername()
{
    return this->servername;
}

std::string Client::getrealname()
{
    return this->realname;
}
std::string Client::getIPaddress()
{
    return this->clientIP;
}

std::string Client::getPrefix()
{
    // Format: :nickname!username@hostname
    return getnickname() + "!" + getusername() + "@" + getIPaddress();
}
int Client::get_clientfd()
{
    return this->clientfd;
}

bool Client::getregistred()
{
    return this->registration_status;
}

bool Client::getoperatorstatus()
{
    return this->Operator_status;
}

bool Client::getlogedstatus()
{
    return this->loged_status;
}

// Client setters:

void Client::setfd(int fd)
{
    this->clientfd = fd;
}
void Client::sethostname(std::string & hostname)
{
    this->hostname = hostname;
}
void Client::setservername(std::string & servername)
{
    this->servername = servername;
}

void Client::setrealname(std::string & realname)
{
    this->realname = realname; 
}
void Client::setnickname( std::string & nickName)
{
    this->nickname = nickName;
}
void Client::setusername(std::string userName)
{
    this->username = userName;
    std::cout << "userName == " << username << "\n\n";

}
void Client::setIPaddress(std::string Ipadd)
{
    this->clientIP = Ipadd;
}

void Client::setregistred(bool reg)
{
    this->registration_status = reg;
}

void Client::setoperatorstatus( bool oper)
{
    this->Operator_status = oper;
}

void Client::setlogedstatus(bool log)
{
    this->loged_status = log;
}

void Client::client_data()
{
    std::cout << " nickname : " << this->nickname << std::endl;
    std::cout << " clientfd : " << this->clientfd << std::endl;
}

