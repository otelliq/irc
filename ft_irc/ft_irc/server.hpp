#pragma once

#include <iostream>
#include <errno.h>
#include <sys/socket.h> // contains definitions of function and structures , socket(), bind(), listen(), and accept(). Defines the sockaddr structure, which is a generic structure for all socket addresses.
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <string>
#include <vector>
#include <netinet/in.h>  // Contains definitions for Internet Protocol family , Defines the sockaddr_in structure, used for IPv4 addresses. , Contains macros like INADDR_ANY and functions like htons() .
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <algorithm>

#include "Channel.hpp"
#include "Client.hpp"
#include "Parcing_data.hpp"

// TOPIc error messages //
#define RPL_NOTOPIC(cli, chan)							(" 331 " + cli + " " + chan + " :No topic is set\r\n")
#define RPL_TOPIC(cli, chan, topic)						(" 332 " + cli + " " + chan + " :" + topic + "\r\n")
#define	RPL_TOPICWHOTIME(user, chan, setter, time)		(" 333 " + user + " " + chan + " "  + setter + " " + time + "\r\n")
#define ERR_INVALIDMODEPARAM(nick, chanuser, mode, param, desc)		(" 696 " + nick + " " + chanuser + " " + mode + " " + param + " " + desc + "\r\n")
#define ERR_CHANOPRIVSNEEDED(client, chan)				(" 482 " + client + " " + chan +  " :You're not channel operator\r\n")
#define RPL_INVITING(client, nick, chan)				(" 341 " + client + " " + nick + " " + chan + "\r\n")
#define ERR_USERONCHANNEL(client, nick, chan)			(" 443 " + client + " " + nick + " " + chan +  " :is already on channel\r\n")

class Client;
class Channel;
class Buffer;

class Server
{
    private:

        int Port;
        std::string Password;
        static int Signal_status;
        int Serverfd;
        int flags_status;
        std::string RawData;
        std::vector<struct pollfd> fds;
        std::vector <std::string> msg;

    public:

        std::vector<Client *> Clients;
        std::vector<Channel *> Channels;

        Server();
        Server(const Server& srv);
        Server &operator=(Server const &src);
        ~Server();

        // Getters
        int getPort();
        int getFd();
        int get_Signal_Status();
        std::string getRawData();
        std::vector<Channel *> get_Channels();

        // Setters
        void setFd(int fd);
        void setPort(int port);
        void setPassword(std::string password);


        void addClient(Client newClient);
        void addChannel(Channel *chan);


        // Server booting functions:
        void Launching_server(int port, std::string password);
        void Server_cycle();


        // Socket functions:
        void socket_creation();
        sockaddr_in socket_infos();
        void socket_Binding(sockaddr_in serveradd);
        void socket_non_blocking();
        void socket_listening();
        // void socket_Accepting();
        void socket_Accepting();
        void server_socket_polling();
        void client_socket_polling(int client_fd);
        void socket_receiving(int client_fd);

        // Signal functions:
        static void Signal_Handler(int signum);

        //Removing functions:
        void Remove_Client(int client_fd);
        void remove_c_from_pollfd(int id);
        void remove_c_from_Vector(int id);
        void Close_filedescriptors();
        void close_all_clients();
        void close_server_socket();
        
        // Parsing received DATA
        void    Parcing_and_Executing(int fd ,std::string buffer);

        std::vector<std::string>  split_received_Buffer(std::string str);
        std::vector<std::string>  split_cmd(std::string &cmd);

        void registerClient(int fd, std::string raw);
        void processMessage(int fd, const std::string& command, const std::string &arg, const std::string &msg);
        Client* findClientByFd(int fd);
        Client* findClientByNick(const std::string& nickname);
        bool isNicknameInUse(const std::string& nickname);
        void sendWelcome(int fd);
        std::string  trim(std::string &str);
        std::vector<std::string> splitByCRLF(const std::string& input);
        Channel * create_channel(Client *cl, std::string name, std::string pass);

        bool Valid_nick_name(std::string& nickname);

        bool Port_valid(std::string port);
        void ssendMessage(std::string message, int destination_fd);


        void    executing_commands(int fd, std::string &cmd);
        Client  *getClient(int fd);
        Client  *getClientname(std::string nickname);
        Channel *getChannel(std::string name);

        // void Server::AddChannel(Channel newChannel);
        // void MODE(Client *admin, std::string mode, std::string arg);
        // void KICK(Client *admin, Client *user, std::string reason);
        // void INVITE(Client *admin, Client *user);
        // void TOPIC(Client *admin, std::string topic);
        // void PART(Client *admin, std::string reason);
        // void NICK(Client *admin, std::string new_nick);
        // void USER(Client *admin, std::string username, std::string realname);
        // void PASS(Client *admin, std::string password);
        // int  PRIVMSG(Client *admin, Client *target, std::string message);
        // void WHO(Client* admin);
        // void Server::AddChannel(Channel newChannel);
        void JOIN(Client* client, std::string &line);
        void MODE( Client *client, std::stringstream &ss);
        void KICK(Client *admin,std::string &line);
        void INVITE(Client *admin,std::string &line);
        void TOPIC(Client *admin,std::string &line);
        void PART(Client *admin,std::string &line);
        void NICK(Client *admin,std::string &line);
        void USER(Client *admin,std::string &line);
        void PASS(Client *admin,std::string &line);
        void  PRIVMSG(Client *admin,std::string &line);
        void WHO(Client* admin, std::string &line);
        
};



