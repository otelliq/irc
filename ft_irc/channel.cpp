/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:53 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/14 19:20:44 by otelliq          ###   ########.fr       */
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
    else if (mode_char == '-'){
        for(; i < mode.size(); ++i){
            if(mode[i] == 't' || mode[i] == 'k' || mode[i] == 'i'){
                if (mode == "i")
                    this->modes += "i";
                else if (mode == "k")
                    this->modes += "k";
                else if (mode == "t")
                    this->modes += "t";
                else if (mode == "o")
                    this->modes += "o";
                else if (mode == "l")
                    this->modes += "l";
                
            }
        }
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