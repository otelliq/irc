/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboudrio <mboudrio@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 00:27:58 by mboudrio          #+#    #+#             */
/*   Updated: 2024/11/28 04:51:53 by mboudrio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int Server::Signal_status = 0;

void Server::Signal_Handler(int signum)
{
    (void)signum;

    std::cout << " Signal " << signum << "received. " << std::endl;
    Signal_status = signum; 
}

void Server::socket_creation()
{
    // 1 - socket creation 
    // socket(domaine , type , protocol);
    Serverfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (Serverfd < 0)
    {
        throw(std::runtime_error(" ---> Socket Creation stage failed "));
    }
}

// void server::getserverIP()
// {
    
// }

sockaddr_in Server:: socket_infos()
{
    // 2 - defining server address  i can use setsockopt also for this 
    sockaddr_in serveradd;
    std::memset(&serveradd, 0, sizeof(serveradd)); // ensure initialisation and clean memory 
    serveradd.sin_family = AF_INET ; // specify addr family;
    serveradd.sin_port = htons(this->Port) ; // specify port number; htons is used to ensure that port numbers are represented in the correct byte order when transmitted over the network 
    // why order matters ? -->   When different computers communicate over a network, they need to agree on how multi-byte values (like port numbers and IP addresses) are represented. Since different computer architectures may use different byte orders, a common standard (network byte order) is used to ensure compatibility.
    serveradd.sin_addr.s_addr = INADDR_ANY ; // it is used when we dont want to bind our network to any particular IP and make it listen to all available IPs
    int enabling_reuse = 1;
    int infos_status = setsockopt(Serverfd, SOL_SOCKET, SO_REUSEADDR, &enabling_reuse, sizeof(enabling_reuse));
    if (infos_status < 0)
        throw(std::runtime_error(" ---> failed to make the local address reusable "));

    return serveradd;
}
void Server::socket_non_blocking()
{
    int non_blocking_status = fcntl(Serverfd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
    {
        throw(std::runtime_error(" ---> failed to make the socket non blocking "));
    }
}
void Server::socket_Binding(sockaddr_in serveradd)
{
    int bind_status = bind(Serverfd , (struct sockaddr*) &serveradd , sizeof(serveradd));
    if (bind_status < 0)
    {
        throw(std::runtime_error(" ---> Binding stage Failed ..."));
    }
}

void Server::socket_listening()
{
    // 4 - listen for incoming connections 
    int listening_status = listen(Serverfd ,  SOMAXCONN);
    if ( listening_status < 0)
    {
        throw(std::runtime_error(" ---> listening stage Failed ..."));
    }
}

void Server::server_socket_polling()
{
    struct pollfd Poll;

    Poll.fd = Serverfd;
    Poll.events = POLLIN; 
    Poll.revents = 0;

    fds.push_back(Poll);
}

void Server::client_socket_polling(int client_fd)
{
    struct pollfd NPoll;

    NPoll.fd = client_fd;
    NPoll.events = POLLIN;
    NPoll.revents = 0;
}

void Server::Launching_server(int port, std::string password)
{
    this->Port = port;
    this->Password = password;
    
    // Socket Operations :
    socket_creation();
    sockaddr_in freemon = socket_infos();
    socket_non_blocking();
    socket_Binding(freemon);
    socket_listening();
    server_socket_polling();

    std::cout << " Your Server Sir is in the Listening mode waiting for incoming connections  ... waiting to accept them" << std::endl;
    
    Server_cycle();
}

void Server::Server_cycle()
{
    while(!get_Signal_Status())
    {
        if((poll(fds.data(), fds.size(), -1) == -1) && get_Signal_Status() == false)
        {
            throw(std::runtime_error(" ---> poll stage failed "));
        }

        size_t i = 0;
        while( i < fds.size())
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == Serverfd)
                    socket_Accepting();
                else 
                    socket_receiving(fds[i].fd);
            }
            i++;
        }
    }
    // Close_filedescriptors();
}


void Server::socket_Accepting()
{
    Client *client = new Client();
    // client = getClient(fd);
    struct sockaddr_in clientadd;
    socklen_t len = sizeof(clientadd);

    int coming_fd = accept(Serverfd, (sockaddr *)&(clientadd), &len);

    if (coming_fd == -1)
        throw(std::runtime_error(" ---> accept stage failed "));

    int non_blocking_status = fcntl(coming_fd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
        throw(std::runtime_error(" ---> failed to make the socket non blocking "));

    // client_socket_polling(coming_fd);

    struct pollfd NPoll;

    NPoll.fd = coming_fd;
    NPoll.events = POLLIN;
    NPoll.revents = 0;
    client->setfd(coming_fd);
    client->setIPaddress(inet_ntoa(clientadd.sin_addr));
    Clients.push_back(client);
    fds.push_back(NPoll);
    
    std::cout << " ---> Client connected " << std::endl;
}


void Server::socket_receiving(int client_fd)
{
    char buffer[1024];
    std::vector<std::string> cmd;
    Buffer Parser;
    Client *client = getClient(client_fd);
    memset(buffer, 0, sizeof(buffer));

    int r = recv(client_fd, buffer, sizeof(buffer), 0);
    RawData = buffer;
    if (r < 0) 
    {
        if (errno == EAGAIN || errno  == EWOULDBLOCK)
            return;
            
        std::cerr << " --> receiving stage failed ... " << std::endl;
        Remove_Client(client_fd);
        close(client_fd);

    }
    else if (r == 0)
    {
        std::cout << " --> Connection closed by Icaruis ..." << std::endl;
        close(client_fd);
    }
    else 
    {
        std::cout << " Received " << r << "  bytes ... " << std::endl;
        std::cout << " Received Data :  " << getRawData() << std::endl;
        std::cout << client->get_clientfd() << std::endl;

        
        // if (!client->getlogedstatus())
        //     registerClient(client_fd, buffer);

        std::cout << "logo" << client->getlogedstatus() << std::endl;
        
        cmd = split_received_Buffer(getRawData());
        // if (client->getlogedstatus())
        // {
            for (size_t i = 0; i < cmd.size(); i++)
            {
                Parcing_and_Executing(client_fd, cmd[i]);
            }
            
        // }
    }
    
    if (std::string(buffer) == "exit") 
    {
        std::cout << "You requested to exit. Closing connection." << std::endl;
        close(client_fd);
        Remove_Client(client_fd);
    }
    
}

std::vector<std::string>  Server::split_received_Buffer(std::string str)
{
    std::vector<std::string> vec;
	std::istringstream stm(str);
	std::string line;
	while(std::getline(stm, line))
	{
		size_t pos = line.find_first_of("\r\n");
		if(pos != std::string::npos)
			line = line.substr(0, pos);
		vec.push_back(line);
	}
	return vec; 
}



void Server::Parcing_and_Executing(int fd, std::string buffer)
{
    // Parser.Parcing_core(buffer);
    executing_commands(fd , buffer);
}

Channel * Server::create_channel(Client *cl, std::string name, std::string pass)
{
    Channel *chan = new Channel(name, pass);
    chan->addAdmin(cl);
    chan->addUser(cl, pass);
    chan->getMembers();
    return chan;
}


void Server::addChannel(Channel *chan) {
    Channels.push_back(chan);
}

std::vector <std::string> Server::split_cmd(std::string &cmd)
{
    std::vector<std::string> vec;
	std::istringstream stm(cmd);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}
void Server::executing_commands(int fd, std::string &cmd)
{   

    size_t found = cmd.find_first_not_of(" \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
        
    Client *client = getClient(fd);
    std::vector<std::string> splited_cmd = split_cmd(cmd);
    
    
    if (cmd.empty())
        return;
    else if (splited_cmd[0] == "PASS")
    {
        PASS(client, cmd);
    }
    else if (splited_cmd[0] == "NICK")
    {
        NICK(client, cmd);
    }
    else if (splited_cmd[0] == "USER")
    {
        USER(client, cmd);
    }
    // else if (splited_cmd[0] == "QUIT")
    // {
    //     QUIT(client, cmd);
    // }
    else if (client->getlogedstatus())
    {
        
        if (splited_cmd[0] == "JOIN")
        {
                JOIN(client, cmd);
        }
        else if (splited_cmd[0] == "WHO")
        {
                WHO(client, cmd);
        }
        else if (splited_cmd[0] == "KICK")
        {
            KICK(client, cmd);
        }
        else if (splited_cmd[0] == "TOPIC")
        {
            TOPIC(client, cmd);
        }
        else if (splited_cmd[0] == "INVITE")
        {
            INVITE(client, cmd);
        }
        else if (splited_cmd[0] == "MODE")
        {
            MODE(client, cmd);
        }
        else if (splited_cmd[0] == "PRIVMSG")
        {
            PRIVMSG(client , cmd); 
        }
        else if (splited_cmd[0] == "PART")
        {
            PART(client , cmd);
        }           
    }
    // else if (client->getlogedstatus())
    // {
        //  send error type not registred !
    // }
    
}

Client* Server::findClientByNick(const std::string& nickname)
{
    for(size_t i = 0; i < Clients.size(); i++)
    {
        if (Clients[i]->getnickname() == nickname)
            return Clients[i];
    }
    return NULL;
}

void Server::ssendMessage(std::string message, int destination_fd)
{
    size_t sent = 0;
    size_t msg_len = message.size();
    char *msg = (char *)message.c_str();
    while(sent < msg_len)
    {
        int i = send(destination_fd, msg + sent, msg_len - sent, 0);
        if(i == -1)
            throw std::runtime_error("send failed");
        sent += i;
    }
}