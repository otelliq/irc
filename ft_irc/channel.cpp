/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:53 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/19 11:08:29 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

channel::channel(){
    name = "default";
    invite_only = false;
    has_password = false;
    has_topic = false;
    has_limit = false;
    operate = false;
}

void channel::set_name(std::string name){
    this->name = name;
}

void channel::setTopic(std::string topic){
    this->topic = topic;
    has_topic = true;
}

void channel::setPassword(std::string password){
    this->password = password;
    has_password = true;
}

void channel::set_MaxUsers(int max){
    this->max_users = max;
    has_limit = true;
}

std::string channel::get_name(){
    return name;
}

bool channel::is_Admin(client *admin){
    if(std::find(admins.begin(), admins.end(), *admin) != admins.end())
        return true;
    return false;
}

void channel::MODE(client *admin, std::string mode, std::string arg){
    std::string reply_message;
    if(!mode.empty()){
        if(is_Admin(admin))
            admin_MODE(admin, mode, arg);
        else{
            reply_message = get_UserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->nickname, this->get_name());
            setbuffer(reply_message, admin->client_fd);
            return;
        }
    }
    else{
        reply_message = get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), this->modes);
        //rpl list of modes
        reply_message += get_UserInfo(admin, false) + " 353 " + admin->nickname + " = " + this->get_name() + " :";
        for(size_t i = 0; i < members.size(); ++i){
            reply_message += "@" + members[i]->nickname + " ";
        }
        reply_message += "\r\n";
        setbuffer(reply_message, admin->client_fd);
        setbuffer(get_UserInfo(admin, false) + RPL_ENDOFWHOIS(admin->nickname, this->get_name()), admin->client_fd);
        setbuffer(reply_message, admin->client_fd);
        reply_message = get_UserInfo(admin, true) + RPL_CREATIONTIME(admin->nickname, this->get_name(), "creation time"); // get creation time
        setbuffer(reply_message, admin->client_fd);
    }
}

void channel::admin_MODE(client *admin, std::string mode, std::string arg){
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
                    reply_message = get_UserInfo(admin, false) + ERR_UNKNOWNMODE(admin->nickname, mode[i]);
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
                reply_message = get_UserInfo(admin, false) + ERR_UNKNOWNMODE(admin->nickname, mode[i]);
                setbuffer(reply_message, admin->client_fd);
            }  
    }
}}

void channel::change_MaxUser(client *admin, int i, std::string &param){
    std::string reply_message;
    int max_users = std::stoi(param);//change this later
    if(i){
        if(param.empty()){
            reply_message = get_UserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +l ");
            setbuffer(reply_message, admin->client_fd);
            return;
    }
        if(max_users <= 0){
            reply_message = get_UserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +l ");
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            has_limit = true;
            set_MaxUsers(max_users);
            send_to_all(get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " +l "));
        }
    }
    else{
        has_limit = false;
        send_to_all(get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " +l "));
    }
}

void channel::changeInviteMode(client *admin, bool i){
    std::string reply_message;
    if(i){
        this->invite_only = true;
        send_to_all(get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " +i" ));
    }
    else{
        this->invite_only = false;
        send_to_all(get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " -i" ));
    }
}
void channel::changeKeyMode(client *admin, std::string key, bool i){
    std::string reply_message;
    if(i){
        if(key.empty()){
            reply_message = get_UserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +k");
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            setPassword(key);
            this->has_password = true;
            reply_message = get_UserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " +k" );
        }
    }
    else{
        this->has_password = false;
        reply_message = get_UserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " -k" );
    }

}

void channel::changeTopicMode(client *admin, bool i){
    std::string reply_message;
    if(i){
        this->has_topic = true;
        send_to_all(get_UserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " +t" ));
    }
    else{
        this->has_topic = false;
        send_to_all(get_UserInfo(admin, false) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " -t" ));
    }
}

void channel::add_admin(client *admin, std::string name){
    this->operate = true;
    std::string reply_message;
    if(name.empty()){
        reply_message = get_UserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->nickname, "MODE" + " +o " + name);
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    client *user = get_user(name);
    if(user){
        admins.push_back(user);
        send_to_all(get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " +o " + admin->nickname));
    }
    else{
        reply_message= get_UserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->nickname, user->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
    }
        
}
void channel::remove_user(client *admin) {
    std::vector<client*>::iterator it = std::find(members.begin(), members.end(), admin);
    if (it != members.end())
        members.erase(it);
    std::vector<client*>::iterator it1 = std::find(admins.begin(), admins.end(), admin);
    if (it1 != admins.end())
        admins.erase(it1);//if anythign goes wrong here, check the iterator
    if(invite_only){
        std::vector<client*>::iterator it2 = std::find(invites.begin(), invites.end(), admin);
        if (it2 != invites.end())
            invites.erase(it2);//if anythign goes wrong here, check the iterator
    }
}
void channel::remove_admin(client *admin, std::string name) {
    this->operate = false;
    std::string reply_message;
    client *user = get_user(name);

    if (user) {
        std::vector<client*>::iterator it = std::find(admins.begin(), admins.end(), admin);

        if (it != admins.end()){
            admins.erase(it);
            send_to_all(get_UserInfo(admin, true) + RPL_CHANNELMODEIS(admin->nickname, this->get_name(), " -o " + admin->nickname));
        }
    }
    else {
        reply_message = get_UserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->nickname, user->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
    }
}

void channel::setbuffer(std::string message, int destination_fd){
    size_t i;
    if(i = send(destination_fd, message.c_str(), message.size(), 0) == -1)
        throw std::runtime_error("send failed");
    if(i != message.size())
        throw std::runtime_error("send failed: not all bytes sent");
}

client *channel::get_user(std::string name){
    for(size_t i = 0; i < members.size(); ++i){
        if(name == members[i]->nickname)
            return members[i];
    }
    return NULL;
}
std::string channel::get_UserInfo(client *admin, bool i){
    if(i)
        return ":" + admin->nickname + "!" + admin->username + "@" + admin->servername + " ";
    else
        return ":" + admin->servername + " ";
}
void channel::send_to_all(std::string message){
    for(size_t i = 0; i < members.size(); ++i){
        setbuffer(message, members[i]->client_fd);
    }
}
void channel::KICK(client *admin, client *user, std::string reason){
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = get_UserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    //isalpha
    if(!is_inChannel(user)){
        reply_message = get_UserInfo(admin, false) + ERR_NOTONCHANNEL(user->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    send_to_all(get_UserInfo(user, true) + "KICK " + this->get_name() + " " + user->nickname + " :" + (reason.empty() ? "bad content" : reason) + "\r\n");
    remove_user(user);
}

bool channel::is_inChannel(client *admin){
    for (std::vector<client *>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (*it == admin) {
            return true;
        }
    }
    return false;
}

void channel::INVITE(client *admin, client *user){
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = get_UserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    if(!is_inChannel(user)){
        reply_message = get_UserInfo(admin, false) + ERR_NOTONCHANNEL(user->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
        return;
    }
    //isalpha
    this->invites.push_back(user);
    setbuffer(get_UserInfo(admin, false) + RPL_INVITING(admin->nickname, user->nickname, this->get_name()), user->client_fd);
    setbuffer(get_UserInfo(admin, false) + RPL_INVITING(admin->nickname, user->nickname, this->get_name()), admin->client_fd);
}

void channel::TOPIC(client *admin, std::string topic_message){
    std::string reply_message;
    std::string error_message;
    if(!is_inChannel(admin)){
        error_message = get_UserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
        setbuffer(error_message, admin->client_fd);
        return;
    }
    if(has_topic){
        if(!topic.empty()){
            setTopic(topic_message);
			reply_message = get_UserInfo(admin, true) + RPL_TOPIC(admin->nickname, this->get_name(), topic_message);
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            reply_message = get_UserInfo(admin, true) + RPL_NOTOPIC(admin->nickname, this->get_name());
            setbuffer(reply_message, admin->client_fd);
            return;
        }
    }
    else{
        if(!topic_message.empty()){
            setTopic(topic_message);
            reply_message = get_UserInfo(admin, true) + RPL_TOPIC(admin->nickname, this->get_name(), topic_message);
            setbuffer(reply_message, admin->client_fd);
            return;
        }
        else{
            reply_message = get_UserInfo(admin, false) + RPL_NOTOPIC(admin->nickname, this->get_name());
            setbuffer(reply_message, admin->client_fd);
            return;
        }
    }
}

void channel::PART(client *admin, std::string reason){
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = get_UserInfo(admin, false) + ERR_NOTONCHANNEL(admin->nickname, this->get_name());
        setbuffer(reply_message, admin->client_fd);
    }
    else{
        remove_user(admin);
		send_to_all(get_UserInfo(admin, true) + " PART " + this->get_name() + " " + reason + "\r\n");
    }
}

//KICK
//INVITE
//TOPIC
//PART
//MODE
