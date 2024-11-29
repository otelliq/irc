/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 05:39:28 by mboudrio          #+#    #+#             */
/*   Updated: 2024/11/29 06:15:27 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Canonical Form:

Channel::Channel(){
    name = "default";
    invite_only = false;
    has_password = false;
    has_topic = false;
    has_limit = false;
    operate = false;
    creation_time = std::time(NULL);
}

Channel::Channel(const std::string& name) : name(name) 
{
    invite_only = false;
    has_password = false;
    has_topic = false;
    has_limit = false;
    operate = false;
    creation_time = std::time(NULL);
}

Channel::Channel(const std::string& name, const std::string& pswd) : name(name), password(pswd) {
    has_password = false;
    invite_only = false;
    has_topic = false;
    has_limit = false;
    operate = false;
    creation_time = std::time(NULL);
}


Channel &Channel::operator=(const Channel &src){
    if(this == &src)
        return *this;
    name = src.name;
    topic = src.topic;
    modes = src.modes;
    password = src.password;
    invite_only = src.invite_only;
    has_password = src.has_password;
    has_topic = src.has_topic;
    has_limit = src.has_limit;
    operate = src.operate;
    max_users = src.max_users;
    Clients = src.Clients;
    admins = src.admins;
    invites = src.invites;
    creation_time = src.creation_time;
    topic_time = src.topic_time;
    return *this;
}

Channel::Channel(Channel const &src){
    *this = src;
}

Channel::~Channel(){
    Clients.clear();
    admins.clear();
    invites.clear();
}

void Channel::SetName(std::string name){
    this->name = name;
}

void Channel::SetTopic(std::string topic){
    this->topic = topic;
    has_topic = true;
}

void Channel::SetPassword(std::string password)
{
	topic.erase(0, 1);
	if (topic.empty())
    {
		has_topic = false;
		topic.clear();
	} 
    else
    {
		topic_time = std::time(NULL);
		has_topic = true;
		topic = topic;
	}
}

std::time_t Channel::get_topictime()
{
    return topic_time;
}
std::string Channel::get_modes()
{
    return modes;
}

void Channel::SetMaxUsers(int max){
    this->max_users = max;
    has_limit = true;
}

void Channel::SetTime(std::time_t time)
{
    this->creation_time = time;
}

void Channel::set_has_password(bool has_password)
{
    this->has_password = has_password;
}


// void Channel::sendMessage(std::string message, int destination_fd){
//     int i;
//     if((i = send(destination_fd, message.c_str(), message.size(), 0)) == -1)
//         throw std::runtime_error("send failed");
//     if(i != (int)message.size())
//         throw std::runtime_error("send failed: not all bytes sent");
// }

void Channel::sendMessage(std::string message, int destination_fd)
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

std::string Channel::GetName(){
    return name;
}

std::time_t Channel::get_time()
{
	return this->creation_time;
}

bool Channel::get_has_password()
{
    return this->has_password;
}

bool Channel::get_has_topic()
{
    return this->has_topic;
}

bool Channel::get_invite_only()
{
    return this->invite_only;
}

bool Channel::get_limit()
{
    return this->has_limit;
}
size_t Channel::get_maxusers()
{
    return this->max_users;
}

std::string Channel::get_password()
{
    return this->password;
}

std::string Channel::get_topic()
{
    return this->topic;
}

//  rah dert wa7ed change hna pookie  zdet & lat9wdha 3lik wla chy haja .
Client *Channel::GetUser(std::string name){
    for(size_t i = 0; i < Clients.size(); ++i){
        if(name == Clients[i]->getnickname())
            return Clients[i];
    }
    return NULL;
}

std::vector<Client *>  Channel::getMembers(){
    return Clients;
}

std::string Channel::GetUserInfo(Client *admin, bool i){
    if(i)
        return ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getservername() + " ";
    else
        return ":" + admin->getservername();
}


void Channel::valid_mode(Client *cli, std::string &modes, std::string param){
    char mode_char = modes[0];
    size_t i = 0;

    if (mode_char == '+')
        i++;
    if (mode_char != '-'){
        for(; i < modes.size(); ++i){
            if(modes[i] == 'i' || modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o' || modes[i] == 't'){
                plus_modes(cli, modes, param);
            }
            else{ 
                //hadchi li lt7t mkhwr
                std::string reply_message = GetUserInfo(cli, false) + ERR_UNKNOWNMODE(cli->getnickname(), modes[i]);
                sendMessage(reply_message, cli->get_clientfd());
            }
        }
    } else {
            for(i = 1; i < modes.size(); ++i){
                if(modes[i] == 'i' || modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o' || modes[i] == 't')
                    minus_modes(cli, modes, param);
                else
                {//ta hadchi mkhwr
                    std::string reply_message = GetUserInfo(cli, false) + ERR_UNKNOWNMODE(cli->getnickname(), modes[i]);
                    sendMessage(reply_message, cli->get_clientfd());
                }

        }
    }
}

void Channel::plus_modes(Client *cli, char c, std::string param){
    if(c == 'i'){
        changeInviteMode(cli, true);
    }
    else if(c == 'k'){
        changeKeyMode(cli, param, true);
    }
    else if(c == 'l'){
        change_MaxUser(cli, 1, param);
    }
    else if(c == 'o'){
        add_admin(cli, param);
    }
    else if(c == 't'){
        changeTopicMode(cli, true);
    }
}

void Channel::minus_modes(Client *cli, char c, std::string param){
    if(c == 'i'){
        changeInviteMode(cli, false);
    }
    else if(c == 'k'){
        changeKeyMode(cli, param, false);
    }
    else if(c == 'l'){
        change_MaxUser(cli, 0, param);
    }
    else if(c == 'o'){
        remove_admin(cli, param);
    }
    else if(c == 't'){
        changeTopicMode(cli, false);
    }
}

void Channel::rpl_topic(Client *cli, std::string topic){
    std::string reply_message;
    if(topic.find(':') == std::string::npos){
        if(onChannel(cli)){
            if(get_has_topic()){
                reply_message = ":" + cli->getnickname() + " " + RPL_TOPIC(cli->getnickname(), this->GetName(), topic);
                sendMessage(reply_message, cli->get_clientfd());
                reply_message = ":" + cli->getnickname() + " " + RPL_TOPICWHOTIME(cli->getnickname(), this->GetName(), cli->getnickname(), std::to_string(get_topictime()));
                sendMessage(reply_message, cli->get_clientfd());
            }
            else{
                reply_message = ":" + cli->getnickname() + " " + RPL_NOTOPIC(cli->getnickname(), this->GetName());
                sendMessage(reply_message, cli->get_clientfd());
            }
        }
        else{
            reply_message = ":" cli->getIPaddress() + ERR_NOTONCHANNEL(cli->getnickname(), this->GetName());
            sendMessage(reply_message, cli->get_clientfd());
        }
        return;
    }
    if(is_Admin(cli)){
        if(modes.find('t') == std::string::npos){
            //topicsetter????
            SetTopic(topic);
			reply_message = ":" + cli->getPrefix() +  " TOPIC " + GetName() + " " + get_topic() + "\r\n";
            send_to_all(reply_message);
        }
        else{
			reply_message = ":" + cli->getIPaddress() + ERR_INVALIDMODEPARAM(cli->getnickname(), GetName(), "TOPIC", get_topic(), "Channel Topic Restrection Are On");
            sendMessage(reply_message, cli->get_clientfd());
            }
    }
    else{
        reply_message = ":" + cli->getIPaddress() + ERR_CHANOPRIVSNEEDED(cli->getnickname(), GetName());
        sendMessage(reply_message, cli->get_clientfd());
        }
}

void Channel::rpl_mode(Client *cli){
    //hadchi rah mahouwach
    sendMessage(GetUserInfo(cli, true) + RPL_CHANNELMODEIS(cli->getnickname(), GetName(), get_modes()), cli->get_clientfd());
    sendMessage(GetUserInfo(cli, true) + RPL_CREATIONTIME(cli->getnickname(), GetName(), std::to_string(get_time())), cli->get_clientfd());
}
void Channel::rpl_list(Client *cli){
    std::string reply_message;

    reply_message = ":" + cli->getIPaddress() + RPL_LIST(cli->getnickname(), GetName(), std::to_string(getMembers().size()), get_topic());
}