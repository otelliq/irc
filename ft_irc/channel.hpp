/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:39 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/15 15:21:19 by otelliq          ###   ########.fr       */
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
        std::vector<client> members;
        std::vector<client> admins;
        std::vector<client> invites;
        channel();
        channel(const channel &c);
        ~channel();
        void setTopic(std::string topic);
        void setPassword(std::string password);
        void set_name(std::string name);
        void set_MODE(std::string mode);
        void set_limit(int limit);
        void set_MaxUsers(int max);
        bool channel::is_Admin(client *admin);
        void channel::admin_MODE(client *admin, std::string mode, std::string arg);
        void channel::sendToSocket(int destination_fd, std::string message);
    
};