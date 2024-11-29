#include "server.hpp"
#include <stdlib.h>
// Canonical Form:

Server::Server()
{
}
Server::Server(const Server &srv)
{
    *this = srv;
}

Server::~Server()
{
}

Server &Server::operator=(Server const &src)
{
    if (this != &src)
    {
        this->Port = src.Port;
        this->Serverfd = src.Serverfd;
        this->Password = src.Password;
        this->Clients = src.Clients;
        // this->Channels = src.Channels;
        this->fds = src.fds;
    }
    return *this;
}

// Server setters :

void Server::setFd(int fd)
{
    this->Serverfd = fd;
}

void Server::setPassword(std::string password)
{
    this->Password = password;
}

void Server::setPort(int port)
{
    this->Port = port;
}

//  Server getters :

std::vector<Channel *> Server::get_Channels()
{
    return this->Channels;
}

int Server::getPort()
{
    return this->Port;
}

std::string Server::getRawData()
{
    return this->RawData;
}

int Server::getFd()
{
    return this->Serverfd;
}

int Server::get_Signal_Status()
{
    return this->Signal_status;
}

Client *Server::getClient(int fd)
{
    size_t i = 0;
    while (i < this->Clients.size())
    {
        if (this->Clients[i]->get_clientfd() == fd)
            return this->Clients[i];
        i++;
    }
    return NULL;
}

Client *Server::getClientname(std::string nickname)
{
    size_t i = 0;
    while (i < this->Clients.size())
    {
        if (this->Clients[i]->getnickname() == nickname)
            return this->Clients[i];
        i++;
    }
    return NULL;
}

Channel *Server::getChannel(std::string name)
{
    size_t i = 0;
    while (i < this->Channels.size())
    {
        if (this->Channels[i]->GetName() == name)
            return Channels[i];
        i++;
    }
    return NULL;
}

// Checks Functions

bool Server::Port_valid(std::string port)
{
    if (port.find_first_not_of("0123456789") != std::string::npos)
    {
        std::cerr << "Invalid character found in port." << std::endl;
        return false;
    }

    int portnum = std::atoi(port.c_str());

    if (portnum >= 1024 && portnum <= 65535)
        return true;
    else
        return false;
}

// Close & Remove Functions

void Server::remove_c_from_pollfd(int id)
{
    size_t i = 0;
    while (i < fds.size())
    {
        if (fds[i].fd == id)
            fds.erase(fds.begin() + i);
        i++;
    }
}
void Server::remove_c_from_Vector(int id)
{
    size_t i = 0;
    while (i < Clients.size())
    {
        if (Clients[i]->get_clientfd() == id)
            Clients.erase(Clients.begin() + i);
        i++;
    }
}
void Server::Remove_Client(int id)
{
    remove_c_from_pollfd(id);
    remove_c_from_Vector(id);
}

void Server::close_all_clients()
{
    size_t i = 0;
    while (i < Clients.size())
    {
        std::cout << " Client " << Clients[i]->get_clientfd() << " Disconnected ..." << std::endl;
        close(Clients[i]->get_clientfd());
        i++;
    }
}

void Server::close_server_socket()
{
    if (Serverfd != -1)
    {
        std::cout << " Server  Disconnected " << std::endl;
        close(Serverfd);
    }
}

void Server::Close_filedescriptors()
{
    close_all_clients();
    close_server_socket();
}

bool Server::Valid_nick_name(std::string& nickname)
{
    size_t i = 0;
    if (!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
        return false;
    while(i < nickname.size() )
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '_')
            return false;
        i++;
    }
    return true;
}


Client *Server::findClientByFd(int fd)
{
    for (std::vector<Client *>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if ((*it)->get_clientfd() == fd)
            return *it;
    }
    return NULL; // Client not found
}

bool Server::isNicknameInUse(const std::string &nickname)
{
    for (std::vector<Client *>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if ((*it)->getnickname() == nickname)
            return true;
    }
    return false;
}

void Server::sendWelcome(int fd)
{
    std::cout << "Client " << fd << " Welcome to the HAKUNA MATATA Realm.\n";
}


std::string Server::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
    return str;
}

std::vector<std::string> Server::splitByCRLF(const std::string &input)
{
    std::vector<std::string> result;
    std::string::size_type start = 0, end;

    while ((end = input.find("\r\n", start)) != std::string::npos)
    {
        result.push_back(input.substr(start, end - start));
        start = end + 2; 
    }

    if (start < input.length())
        result.push_back(input.substr(start));

    return result;
}


void Client::sendError(Client *client, const std::string& errorCode, const std::string& channel, const std::string& message)
{
    std::string errorMsg;

    // JOIN ERRORS //
    if (errorCode == "403")
    {  // ERR_NOSUCHCHANNEL
        errorMsg = "403 " + client->getnickname() + " " + channel + " :No such channel\r\n";
    }

    else if (errorCode == "471") 
    {  // ERR_CHANNELISFULL
        errorMsg = "471 " + client->getnickname() + " " + channel + " :Cannot join channel (+l)\r\n";
    }
    else if (errorCode == "405") 
    {  // ERR_TOOMANYCHANNELS
        errorMsg = "405 " + client->getnickname() + " " + channel + " :You have joined too many channels\r\n";
    }

    else if (errorCode == "471") 
    {  // ERR_CHANNELISFULL
        errorMsg = "471 " + client->getnickname() + " " + channel + " :Cannot join channel (+l)\r\n";
    }

    else if (errorCode == "473") 
    {  // ERR_INVITEONLYCHAN
        errorMsg = "473 " + client->getnickname() + " " + channel + " :Cannot join channel (+i)\r\n";
    }

    else if (errorCode == "474") 
    {  // ERR_BANNEDFROMCHAN
        errorMsg = "474 " + client->getnickname() + " " + channel + " :Cannot join channel (+b)\r\n";
    }

    else if (errorCode == "475") 
    {  // ERR_BADCHANNELKEY
        errorMsg = "475 " + client->getnickname() + " " + channel + " :Cannot join channel (+k)\r\n";
    }

    // PRIVEMSG ERRORS //
    else if (errorCode == "404")
    {  // ERR_CANNOTSENDTOCHAN
        errorMsg = "404 " + client->getnickname() + " " + channel + " :Cannot send to channel\r\n";
    }
    
    // the command they sent isn’t known by the server //
    else if (errorCode == "421") 
    {  // ERR_UNKNOWNCOMMAND
        errorMsg = "421 " + client->getnickname() + " " + message + " :Unknown command\r\n";
    }

    // PASS //

    else if (errorCode == "464")
    {  //ERR_PASSWDMISMATCH
        errorMsg = "464 " + client->getnickname() + " " + message + " :Password incorrect\r\n";
    }

    // NICK //
    else if (errorCode == "432") 
    {  // ERR_ERRONEUSNICKNAME
        errorMsg = "432 " + client->getnickname() + " " + message + " :Erroneus nickname\r\n";
    }
    else if (errorCode == "433") 
    {  // ERR_NICKNAMEINUSE
        errorMsg = "433 " + client->getnickname() + " " + message + " :Nickname is already in use\r\n";
    }

    // KICK //
    else if (errorCode == "441") 
    {  // ERR_USERNOTINCHANNEL
        errorMsg = "441 " + client->getnickname() + " " + message + " :They aren't on that channel\r\n";
    }

    else if (errorCode == "442") 
    {  // ERR_NOTONCHANNEL
        errorMsg = "442 " + client->getnickname() + " " + message + " :You're not on that channel\r\n";
    }

    // NEED MORE PARAMS //
    else if (errorCode == "461") 
    {  // ERR_NEEDMOREPARAMS
        errorMsg = "461 " + client->getnickname() + " " + message + " :Not enough parameters\r\n";
    }

    // Registration system //checked
    // Returned when a client->tries to change a detail that can only be set during registration (such as resending the PASS or USER after registration).
    else if (errorCode == "462") 
    {  // ERR_ALREADYREGISTERED
        errorMsg = "462 " + client->getnickname() + message + " :You may not reregister\r\n";
    }

    else if (errorCode == "451") 
    {  // ERR_NOTREGISTERED
        errorMsg = "451 " + client->getnickname() + " " + message + " :You have not registered\r\n";
    }

    else if (errorCode == "1")
    {   // Replies :
        errorMsg =  client->getnickname() + " " + message + "  \r\n";  
    }


    else {
        errorMsg = errorCode + " :Unknown error\r\n";
    }

    // Sending the error message to the client
    send(client->get_clientfd(), errorMsg.c_str(), errorMsg.length(), 0);
}

