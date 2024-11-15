/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 12:13:24 by otelliq           #+#    #+#             */
/*   Updated: 2024/11/15 15:27:31 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>

class client{
    public:
        int client_fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string hostname;
        std::string servername;
        std::string ip;
        std::string command;
        bool is_registered;
        bool has_password;
        client();
};