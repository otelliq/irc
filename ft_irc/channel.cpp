/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:53 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/20 20:22:53 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

Channel::Channel(){
    name = "default";
    invite_only = false;
    has_password = false;
    has_topic = false;
    has_limit = false;
    operate = false;
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

void Channel::SetPassword(std::string password){
    this->password = password;
    has_password = true;
}

void Channel::SetMaxUsers(int max){
    this->max_users = max;
    has_limit = true;
}

void Channel::SetTime(std::string time)
{
    this->creation_time = time;
}


void Channel::setbuffer(std::string message, int destination_fd){
    int i;
    if((i = send(destination_fd, message.c_str(), message.size(), 0)) == -1)
        throw std::runtime_error("send failed");
    if(i != (int)message.size())
        throw std::runtime_error("send failed: not all bytes sent");
}

std::string Channel::GetName(){
    return name;
}

std::string Channel::get_time(){
	return this->creation_time;
}


client *Channel::GetUser(std::string name){
    for(size_t i = 0; i < Clients.size(); ++i){
        if(name == Clients[i]->nickname)
            return Clients[i];
    }
    return NULL;
}

std::string Channel::GetUserInfo(client *admin, bool i){
    if(i)
        return ":" + admin->nickname + "!" + admin->username + "@" + admin->servername + " ";
    else
        return ":" + admin->servername + " ";
}

bool Channel::is_Admin(client *admin){
    if(std::find(admins.begin(), admins.end(), admin) != admins.end())
        return true;
    return false;
}

bool Channel::is_inChannel(client *admin){
    for (std::vector<client *>::const_iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (*it == admin) {
            return true;
        }
    }
    return false;
}


void Channel::send_to_all(std::string message){
    for(size_t i = 0; i < Clients.size(); ++i){
        setbuffer(message, Clients[i]->client_fd);
    }
}


void Channel::admin_MODE(client *admin, std::string mode, std::string arg){
    std::string reply_message;
    char mode_char = mode[0];
    size_t i = 0;
    if (mode_char == '+')
        i++;
    if (mode_char != '-'){
        for(; i < mode.size(); ++i){
                if (mode == "i"){
                    this->modes += "i";
                    changeInviteMode(admin, true);
                }
                else if (mode == "k"){
                    this->modes += "k";
                    changeKeyMode(admin, arg, true);
                }
                else if (mode == "t"){
                    this->modes += "t";
                    changeTopicMode(admin, true);
                }
                else if (mode == "o"){
                    this->modes += "o";
                    add_admin(admin, arg);
                }
                else if (mode == "l"){
                    this->modes += "l";
                    change_MaxUser(admin, 1, arg);
                }
                else{
                    reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->nickname, mode[i]);
                    setbuffer(reply_message, admin->client_fd);
                }
        }
    }
    else{
        for(; i < mode.size(); ++i){
            if (mode == "i"){
                this->modes += "i";
                changeInviteMode(admin, false);
            }
            else if (mode == "k"){
                this->modes += "k";
                changeKeyMode(admin, arg, false);
            }
            else if (mode == "t"){
                this->modes += "t";
                changeTopicMode(admin, false);
            }
            else if (mode == "o"){
                this->modes += "o";
                remove_admin(admin, arg);
            }
            else if (mode == "l"){
                this->modes += "l";
                change_MaxUser(admin, 0, arg);
            }
            else{
                reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->nickname, mode[i]);
                setbuffer(reply_message, admin->client_fd);
            }  
        }
    }
}

void Channel::change_MaxUser(client *admin, int i, std::string &param){
    std::string reply_message;
    int max_users = std::stoi(param);//change this later
    if(i){
        if(param.empty()){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +l ");
            setbuffer(reply_message, admin->client_fd);
            return;
    }
        if(max_users <= 0){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +l ");
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            has_limit = true;
            SetMaxUsers(max_users);
            send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " +l "));
        }
    }
    else{
        has_limit = false;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " +l "));
    }
}

void Channel::changeInviteMode(client *admin, bool i){
    std::string reply_message;
    if(i){
        this->invite_only = true;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " +i" ));
    }
    else{
        this->invite_only = false;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " -i" ));
    }
}

void Channel::changeKeyMode(client *admin, std::string key, bool i){
    std::string reply_message;
    if(i){
        if(key.empty()){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +k");
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            SetPassword(key);
            this->has_password = true;
            reply_message = GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " +k" );
        }
    }
    else{
        this->has_password = false;
        reply_message = GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " -k" );
    }

}

void Channel::changeTopicMode(client *admin, bool i){
    std::string reply_message;
    if(i){
        this->has_topic = true;
        send_to_all(GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " +t" ));
    }
    else{
        this->has_topic = false;
        send_to_all(GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " -t" ));
    }
}

void Channel::add_admin(client *admin, std::string name){
    this->operate = true;
    std::string reply_message;
    if(name.empty()){
        reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +o " + name);
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    client *user = GetUser(name);
    if(user){
        admins.push_back(user);
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " +o " + admin->nickname));
    }
    else{
        reply_message= GetUserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->nickname, user->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
    }
        
}

void Channel::remove_user(client *admin) {
    std::vector<client*>::iterator it = std::find(Clients.begin(), Clients.end(), admin);
    if (it != Clients.end())
        Clients.erase(it);
    std::vector<client*>::iterator it1 = std::find(admins.begin(), admins.end(), admin);
    if (it1 != admins.end())
        admins.erase(it1);//if anythign goes wrong here, check the iterator
    if(invite_only){
        std::vector<client*>::iterator it2 = std::find(invites.begin(), invites.end(), admin);
        if (it2 != invites.end())
            invites.erase(it2);//if anythign goes wrong here, check the iterator
    }
}

void Channel::remove_admin(client *admin, std::string name) {
    this->operate = false;
    std::string reply_message;
    client *user = GetUser(name);

    if (user) {
        std::vector<client*>::iterator it = std::find(admins.begin(), admins.end(), admin);

        if (it != admins.end()){
            admins.erase(it);
            send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), " -o " + admin->nickname));
        }
    }
    else {
        reply_message = GetUserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->nickname, user->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
    }
}

void Channel::MODE(client *admin, std::string mode, std::string arg){
    std::string reply_message;
    if(!mode.empty()){
        if(is_Admin(admin))
            admin_MODE(admin, mode, arg);
        else{
            reply_message = GetUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->GetName());
            setbuffer(reply_message, admin->client_fd);
            return;
        }
    }
    else{
        reply_message = GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->GetName(), this->modes);
        //rpl list of modes
        reply_message += GetUserInfo(admin, false) + " 353 " + admin->nickname + " = " + this->GetName() + " :";
        for(size_t i = 0; i < Clients.size(); ++i){
            reply_message += "@" + Clients[i]->nickname + " ";
        }
        reply_message += "\r\n";
        setbuffer(reply_message, admin->client_fd);
        setbuffer(GetUserInfo(admin, false) + RPL_ENDOFWHOIS(admin->nickname, this->GetName()), admin->client_fd);
        setbuffer(reply_message, admin->client_fd);
        reply_message = GetUserInfo(admin, true) + RPL_CREATIONTIME(admin->nickname, this->GetName(), this->get_time);
        setbuffer(reply_message, admin->client_fd);
    }
}

void Channel::KICK(client *admin, client *user, std::string reason){
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    //isalpha
    if(!is_inChannel(user)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(user->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    send_to_all(GetUserInfo(user, true) + "KICK " + this->GetName() + " " + user->nickname + " :" + (reason.empty() ? "bad content" : reason) + "\r\n");
    remove_user(user);
}


void Channel::INVITE(client *admin, client *user){
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    if(!is_inChannel(user)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(user->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    //isalpha
    this->invites.push_back(user);
    setbuffer(GetUserInfo(admin, false) + RPL_INVITING(admin->nickname, user->nickname, this->GetName()), user->client_fd);
    setbuffer(GetUserInfo(admin, false) + RPL_INVITING(admin->nickname, user->nickname, this->GetName()), admin->client_fd);
}

void Channel::TOPIC(client *admin, std::string topic_message){
    std::string reply_message;
    std::string error_message;
    if(!is_inChannel(admin)){
        error_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->GetName());
        setbuffer(error_message, admin->client_fd);
        return;
    }
    if(has_topic){
        if(!topic.empty()){
            SetTopic(topic_message);
			reply_message = GetUserInfo(admin, true) + RPL_TOPIC(admin->nickname, this->GetName(), topic_message);
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            reply_message = GetUserInfo(admin, true) + RPL_NOTOPIC(admin->nickname, this->GetName());
            setbuffer(reply_message, admin->client_fd);
            return;
        }
    }
    else{
        if(!topic_message.empty()){
            SetTopic(topic_message);
            reply_message = GetUserInfo(admin, true) + RPL_TOPIC(admin->nickname, this->GetName(), topic_message);
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            reply_message = GetUserInfo(admin, false) + RPL_NOTOPIC(admin->nickname, this->GetName());
            setbuffer(reply_message, admin->client_fd);
            return;
        }
    }
}

void Channel::PART(client *admin, std::string reason){
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->GetName());
        setbuffer(reply_message, admin->client_fd);
    }
    else{
        remove_user(admin);
		send_to_all(GetUserInfo(admin, true) + " PART " + this->GetName() + " " + reason + "\r\n");
    }
}

/////////////////////////////////////

// void Channel::NICK(client *admin, const std::string new_nick) {//reference--------

//     for (size_t i = 0; i < Clients.size(); ++i) {
//         if (Clients[i]->nickname == new_nick) {
//             std::string message = GetUserInfo(admin, false) + " " + admin->nickname + " " + new_nick + ":Nickname is already in use\r\n";
//             send(admin->client_fd, message.c_str(), message.size(), 0);
//             return;
//         }
//     }

//     admin->nickname = new_nick;
// }

// void Channel::USER(client *admin, std::string username, std::string realname) {
//     if(realname.find(":")){
//         std::string message = ERR_NEEDMOREPARAMS1();
//         send(admin->client_fd, message.c_str(), message.size(), 0);
//     }
//     else{
//         admin->username = username;
//         admin->realname = realname;
//     }
    
// }

// void Channel::PASS(client *admin, std::string password) {
//     // if(password == this->server.password)//ta nmergiw w assigni hada l password d server
//         admin->has_password = true;
//     // else{
//         std::string message = ERR_PASSWDMISMATCH();
//         send(admin->client_fd, message.c_str(), message.size(), 0);
//     // }
// }

int Channel::PRIVMSG(client *admin, client *target, std::string message) {
    if (message.find("#") == 0){
        if (!this->onChannel(admin)){
            std::string error_msg = ERR_NOTONCHANNEL(admin->nickname, this->GetName());
            send(admin->client_fd, error_msg.c_str(), error_msg.length(), 0);
            return -1;
        }
        for (size_t i = 0; i < Clients.size(); ++i) {
            if (Clients[i]->nickname == admin->nickname) {
                continue;
            }
            std::string msg_to_send = ":" + admin->nickname + "!" + admin->username + "@localhost PRIVMSG " + this->GetName() + " :" + message + "\r\n";
            send(Clients[i]->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
        }
        return 0;
    }

    if (target == NULL || target->nickname.empty()){
        std::string error_msg = ERR_NOSUCHNICK(admin->nickname, "unknown");
        send(admin->client_fd, error_msg.c_str(), error_msg.length(), 0);
        return -1;
    }

    std::string msg_to_send = ":" + admin->nickname + "!" + admin->username + "@localhost PRIVMSG " + target->nickname + " :" + message + "\r\n";
    send(target->client_fd, msg_to_send.c_str(), msg_to_send.length(), 0);
    return 0;
}
