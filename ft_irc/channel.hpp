/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:39 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/18 00:15:27 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include "client.hpp"
class client;

class channel{
    private:
        std::string name;
        std::string topic;
        std::string password;
        std::string modes;
        bool invite_only;
        bool has_password;
        bool has_topic;
        bool has_limit;
        bool operate;
        int max_users;
    public:
        std::vector<client *> members;
        std::vector<client *> admins;
        std::vector<client *> invites;
        channel();
        channel(const channel &c);
        ~channel();
        void setTopic(std::string topic);
        void setPassword(std::string password);
        void set_name(std::string name);
        void set_MODE(std::string mode);
        void set_limit(int limit);
        void set_MaxUsers(int max);
        void set_Admin(client *param);
        bool is_Admin(client *admin);
        void admin_MODE(client *admin, std::string mode, std::string arg);
        // void sendToSocket(int destination_fd, std::string message);
        void setbuffer(std::string message, int destination_fd);
        void changeInviteMode(client *admin, bool i);
        void changeKeyMode(client *admin, std::string key, bool i);
        void changeTopicMode(client *admin, bool i);
        void add_admin(client *admin, std::string name);
        void remove_admin(client *admin, std::string name);
        void change_MaxUser(client *admin, int i, std::string &param);
        client *get_user(std::string name);
    
};