/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 18:22:39 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/18 22:37:37 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include "client.hpp"

// TOPIc error messages //
#define RPL_NOTOPIC(cli, chan)							(" 331 " + cli + " " + chan + " :No topic is set\r\n")
#define RPL_TOPIC(cli, chan, topic)						(" 332 " + cli + " " + chan + " :" + topic + "\r\n")

#define RPL_WELCOME()									(" 001 :Welcome to the Internet Relay Network \r\n")
#define ERR_PASSWDMISMATCH()							(" 464 :Password incorrect\r\n")
#define RPL_ENDOFWHOIS(cli, chan)						(" 315 " + cli + " " + chan + " :End of /WHOIS list\r\n")
#define RPL_ENDOFNAMES(cli, channel)					(" 366 " + cli + " " + chan + " :End of /NAMES list\r\n")
#define ERR_NOSUCHNICK(client,nick)						(" 401 "  + client + " " + nick + ":No such nick/channel\r\n")

// CHANNEL error messages //
#define ERR_NEEDMOREPARAMS(client, command)				(" 461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_NEEDMOREPARAMS1()							(" 461 :Not enough parameters\r\n")
#define ERR_USERNOTINCHANNEL(client, nick, chan) 		(" 441 " + client + " " + nick + " " + chan + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, chan)  				(" 442 " + client + " " + chan +  " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(client, nick, chan)			(" 443 " + client + " " + nick + " " + chan +  " :is already on channel\r\n")
#define ERR_NOSUCHCHANNEL(client, chan)	   				(" 403 " + client + " " + chan + " :No such channel\r\n")
#define ERR_UNKNOWNCOMMAND(client, command)				(" 421 " + client + " " + command + " :Unknown command\r\n")

// MODE error messages //
#define ERR_UNKNOWNMODE(nick, mode)						(" 472 " + nick + " " + mode + " :is unknown mode char to me\r\n")
#define	RPL_CHANNELMODEIS(cli, chan, mode)				(" 324 " + cli + " " + chan + " " + mode + "\r\n")
#define RPL_CREATIONTIME(cli, chan, creationTime)   	(" 329 " + cli + " " + chan + " " + creationTime + "\r\n")
#define ERR_CHANOPRIVSNEEDED(client, chan)				(" 482 " + client + " " + chan +  " :You're not channel operator\r\n")
#define RPL_INVITING(client, nick, chan)				(" 341 " + client + " " + nick + " " + chan + "\r\n")

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
        std::string get_name();
        bool is_Admin(client *admin);
        bool onChannel(client *admin);
        void admin_MODE(client *admin, std::string mode, std::string arg);
        void MODE(client *admin, std::string mode, std::string arg);
        void setbuffer(std::string message, int destination_fd);
        void changeInviteMode(client *admin, bool i);
        void changeKeyMode(client *admin, std::string key, bool i);
        void changeTopicMode(client *admin, bool i);
        void add_admin(client *admin, std::string name);
        void remove_admin(client *admin, std::string name);
        void change_MaxUser(client *admin, int i, std::string &param);
        client *get_user(std::string name);
        std::string get_UserInfo(client *admin, bool i);
        void send_to_all(std::string message);
        void KICK(client *admin, client *user, std::string reason);
        bool is_inChannel(client *admin);
        void remove_user(client *admin);
    
};