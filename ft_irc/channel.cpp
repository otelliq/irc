/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:53 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/15 15:58:47 by otelliq          ###   ########.fr       */
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
bool channel::is_Admin(client *admin){
    if(std::find(admins.begin(), admins.end(), *admin) != admins.end())
        return true;
    return false;
}
void channel::MODE(client *admin, std::string mode, std::string arg){
    if(!mode.empty()){
        if(is_Admin(admin)){
            
        }
    }
}

void channel::admin_MODE(client *admin, std::string mode, std::string arg){
    std::string reply_message;
    char mode_char = mode[0];
    size_t i = 0;
    if (mode_char == '+')
        i++;
    if (mode_char == '-'){
        for(; i < mode.size(); ++i){
                if (mode == "i"){
                    this->modes += "i";
                    //change invite mode
                }
                else if (mode == "k"){
                    this->modes += "k";
                    //change key mode
                }
                else if (mode == "t"){
                    this->modes += "t";
                    //change topic mode
                }
                else if (mode == "o"){
                    this->modes += "o";
                    //add admin
                }
                else if (mode == "l"){
                    this->modes += "l";
                    //change limit 
                }
                else{
                    reply_message = "Invalid mode";//send error message
                    sendToSocket(admin->client_fd, reply_message);
                }
        }
    }
    else{
        for(; i < mode.size(); ++i){
            if (mode == "i"){
                this->modes += "i";
                //change invite mode
            }
            else if (mode == "k"){
                this->modes += "k";
                //change key mode
            }
            else if (mode == "t"){
                this->modes += "t";
                //change topic mode
            }
            else if (mode == "o"){
                this->modes += "o";
                //add admin
            }
            else if (mode == "l"){
                this->modes += "l";
                //change limit 
            }
            else{
                reply_message = "Invalid mode";//send error message
                sendToSocket(admin->client_fd, reply_message);
            }  
    }
}

void channel::sendToSocket(int destination_fd, std::string message){
    size_t total_sent = 0;
    size_t len = message.size();
    const char *msg = message.c_str();

    while(total_sent < len){
        int sent = send(destination_fd, msg + total_sent, len - total_sent, 0);
        if(sent == -1)
            throw std::runtime_error("Error sending message to socket");
    }
}

void channel::changeInviteMode(client *admin, bool i){
    std::string reply_message;
    if(i){
        this->invite_only = true;
        reply_message = "Invite only mode is now on";
    }
    else{
        this->invite_only = false;
        reply_message = "Invite only mode is now off";
    }
}
// void channel::set_MODE(std::string mode){
//     if (mode == "i")
//         return;
//     else if (mode == "k")
//         return;
//     else if (mode == "t")
//         return;
//     else if (mode == "o")
//         return;
//     else if (mode == "l")
//         return;
// }