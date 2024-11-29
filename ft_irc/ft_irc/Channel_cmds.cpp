
#include "Channel.hpp"



bool Channel::is_Admin(Client *admin){
    if(std::find(admins.begin(), admins.end(), admin) != admins.end())
        return true;
    return false;
}

bool Channel::is_Invited(Client *admin)
{
    if(std::find(invites.begin(), invites.end(), admin) != admins.end())
        return true;
    return false;
}


bool Channel::is_inChannel(Client *admin)
{
    for (std::vector<Client *>::const_iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (*it == admin)
        {
            return true;
        }
    }
    return false;
}


void Channel::send_to_all(std::string message)
{
    for(size_t i = 0; i < Clients.size(); ++i)
    {
        sendMessage(message, Clients[i]->get_clientfd());
    }
}


void Channel::admin_MODE(Client *admin, std::string mode, std::string arg){
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
                    reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->getnickname(), mode[i]);
                    sendMessage(reply_message, admin->get_clientfd());
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
                remove_admin(admin);
            }
            else if (mode == "l"){
                this->modes += "l";
                change_MaxUser(admin, 0, arg);
            }
            else{
                reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->getnickname(), mode[i]);
                sendMessage(reply_message, admin->get_clientfd());
            }  
        }
    }
}

void Channel::change_MaxUser(Client *admin, int i, std::string &param){
    std::string reply_message;
    int max_users = std::stoi(param);//change this later
    if(i){
        if(param.empty()){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +l ");
            sendMessage(reply_message, admin->get_clientfd());
            return;
    }
        if(max_users <= 0){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +l ");
            sendMessage(reply_message, admin->get_clientfd());
            return;
        }
        else{
            has_limit = true;
            SetMaxUsers(max_users);
            send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +l "));
        }
    }
    else{
        has_limit = false;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +l "));
    }
}

void Channel::changeInviteMode(Client *admin, bool i){
    std::string reply_message;
    if(i){
        this->invite_only = true;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +i" ));
    }
    else{
        this->invite_only = false;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -i" ));
    }
}

void Channel::changeKeyMode(Client *admin, std::string key, bool i){
    std::string reply_message;
    if(i){
        if(key.empty()){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +k");
            sendMessage(reply_message, admin->get_clientfd());
            return;
        }
        else{
            SetPassword(key);
            this->has_password = true;
            reply_message = GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +k" );
        }
    }
    else{
        this->has_password = false;
        reply_message = GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -k" );
    }

}

void Channel::changeTopicMode(Client *admin, bool i){
    std::string reply_message;
    if(i){
        this->has_topic = true;
        send_to_all(GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +t" ));
    }
    else{
        this->has_topic = false;
        send_to_all(GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -t" ));
    }
}

void Channel::remove_Invited(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(invites.begin(), invites.end(), admin);
    if (it != invites.end()) {
        invites.erase(it);
    }
}

void Channel::remove_admin(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(admins.begin(), admins.end(), admin);
    if (it != admins.end()) {
        admins.erase(it);
    }
}

void Channel::remove_user(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(invites.begin(), invites.end(), admin);
    if (it != invites.end()) {
        invites.erase(it);
    }
}


void Channel::add_admin(Client *admin, std::string name){
    this->operate = true;
    std::string reply_message;
    if(name.empty())
    {
        reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +o " + name);
        sendMessage(reply_message, admin->get_clientfd());
        return;
    }
    Client *user = GetUser(name);
    if(user){
        admins.push_back(user);
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +o " + admin->getnickname()));
    }
    else{
        reply_message= GetUserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->getnickname(), user->getnickname(), this->GetName());
        sendMessage(reply_message, admin->get_clientfd());
    }
        
}
void Channel::addUser(Client* client, std::string pass)
{
    if (!client)
     {
        std::cout << "Client pointer is null in addUser!" << std::endl;
    }

    if (has_password)
    {
        if (pass == password)
        {
            std::string rpl = client->getPrefix() + " JOIN " + name + " * :realname\r\n";
            std::cout << "madkoukin hna2\n";
            
            if (std::find(Clients.begin(), Clients.end(), client) == Clients.end())
            {
                Clients.push_back(client);
                send_to_all(rpl);
            } 
            else 
            {
                std::cerr << "Client already exists in the channel!" << std::endl;
            }
        } else {
            std::cerr << "Invalid Password" << std::endl;
        }
    } else
    {
        // :mboudrio!mboudrio@mboudrio JOIN #some * :realnam
        // :mboudrio!mboudrio@88ABE6.25BF1D.D03F86.88C9BD.IP JOIN #some * :realna
        // :mboudrio!mboudrio@88ABE6.25BF1D.D03F86.88C9BD.IP MODE #some -o mboudr
        std::string rpl = ":" +  client->getPrefix() + " JOIN " + name + " * :realname\r\n";
        std::cout << "madkoukin hna\n";
        if (std::find(Clients.begin(), Clients.end(), client) == Clients.end())
        {
            Clients.push_back(client);
            send_to_all(rpl);
        } else {
            std::cerr << "Client already exists in the channel!" << std::endl;
        }
    }
}
void Server::JOIN(Client* client, std::string& line)
{
    std::string cmd , chan_name , pass;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, pass);
    if (chan_name[0] == '#') 
    { 
        Channel *chan = getChannel(chan_name); 
         if (chan == NULL)
        {

            chan = create_channel(client, chan_name , pass);
             addChannel(chan);
        } 
        else 
        {
                    chan->addUser(client, pass);
        }
    }
}

void Server::PASS(Client * client , std::string & line)
{

    std::string cmd, password;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> password;


    if (password.empty())
        client->sendError(client , "461" , "" , " u need to enter a Password to acces the server ");

    else if (!client->getregistred())
    {
        if (password == Password)
        {
            client->setregistred(true);
        }
        else
        {
            client->sendError(client , "464", "" , "ERR_PASSWDMISMATCH");
        }
    }
    else 
        client->sendError(client, "462" , "" , "ERR_ALREADYREGISTERED");
}

void Server::NICK(Client * client , std::string & line)
{
    std::istringstream ss(line);
    std::string nickname , cmd;
    ss >> cmd;
    ss >> nickname;

    std::string granpa; 
    if (nickname.empty())
        client->sendError(client , "461" , "" , " u need to enter a Nickname to acces the server ");
    if (isNicknameInUse(nickname) && client->getnickname() != nickname)
        client->sendError(client , "433", "" , "ERR_NICKNAMEINUSE");
    if (!Valid_nick_name(nickname))
        client->sendError(client , "432", "" , "ERR_ERRONEUSNICKNAME");
    if (client->getregistred())
    {
        granpa = client->getnickname();
        client->setnickname(nickname);
        if (!granpa.empty() && granpa != nickname)
        {
            if (granpa == "212" && !client->getusername().empty())
            {
                client->setlogedstatus(true);
                client->sendError(client , "1", "", "RPL_NICKCHANGE");
            }
            else
            {
                client->sendError(client , "1", "", "RPL_NICKCHANGE");
            }
    }
    }
    else if (!client->getregistred())
            client->sendError(client , "451", "" , "ERR_NOTREGISTERED2");
    if (client->getregistred() && !client->getusername().empty() && !client->getnickname().empty() && client->getnickname() != "212" && !client->getlogedstatus())
    {
        std::cout << "dkhalt hna o " << client->getnickname() << " howa l nick" << std::endl;
        client->setnickname(nickname);
    }
}

void Server::USER(Client * client , std::string & line)
{
    std::istringstream ss(line);
    std::string username, hostname, servername, realname;
    ss >> username;
    ss >> hostname;
    ss >> servername;
    ss >> realname;

    std::cout << "\nip == " << servername << "xddd\n xdd\n";
    if (username.empty() || realname.empty() || servername.empty() || hostname.empty())
    {   
        client->sendError(client , "461" , "" , " u need to enter a username,realname,servername,hostname  to acces the server ");
    }
    else if (!client->getregistred())
    {
        client->sendError(client , "451", "" , "ERR_NOTREGISTERED1");
    }   
    else if ( client->getnickname().empty() || username.empty() || realname.empty() || servername.empty() || hostname.empty())
    {
        client->sendError(client, "462" , "" , "ERR_ALREADYREGISTERED1");
    }   
    else
    {
        client->setusername(username);
        client->setservername(servername);
        client->setrealname(realname);
        client->sethostname(hostname);  
    }
    if (client->getregistred() && !client->getusername().empty() && !client->getrealname().empty() && !client->getservername().empty() && !client->gethostname().empty() && !client->getnickname().empty())
    {
        client->setlogedstatus(true);
    }
            
    else
    {
        client->sendError(client, "421" , "" , "ERR_UNKNOWNCOMMAND :  To register  1/PASS 2/NICK /USER");
    }

    if (client->getregistred() && client->getnickname().size() > 0 && client->getusername().size() > 0)
        sendWelcome(client->get_clientfd());
}



// void Channel::addUser(Client* client, std::string pass)
// {
//     if (has_password) 
//     {
//         if (pass == password)
//         {
//             std::string rpl;
//             // :mboudrio!mboudrio@88ABE6.25BF1D.D03F86.88C9BD.IP JOIN #some * :realname
//             rpl = client->getPrefix() + " JOIN " + name + " * :realname\r\n";
//             send_to_all(rpl);
//             Clients.push_back(client);
//             // valid_pass(pass)
//         }
//         else 
//             std::cout << "Invalid Password" << std::endl;
//     }
//     else 
//     {
//         std::string rpl;

//         // :mboudrio!mboudrio@88ABE6.25BF1D.D03F86.88C9BD.IP JOIN #some * :realname

//         rpl = client->getPrefix() + " JOIN " + name + " * :realname\r\n";
//         send_to_all(rpl);
//         Clients.push_back(client);
//     }
// }

// void Channel::addAdmin(Client* client)
// {
//         client->setoperatorstatus(true);
//         admins.push_back(client); 
// }
// void Channel::addInvited(Client* client) {
//     invites.push_back(client);
// }


// void Channel::remove_user(Client *admin) {
//     std::vector<Client*>::iterator it = std::find(Clients.begin(), Clients.end(), admin);
//     if (it != Clients.end())
//         Clients.erase(it);
//     std::vector<Client*>::iterator it1 = std::find(admins.begin(), admins.end(), admin);
//     if (it1 != admins.end())
//         admins.erase(it1);//if anythign goes wrong here, check the iterator
//     if(invite_only){
//         std::vector<Client*>::iterator it2 = std::find(invites.begin(), invites.end(), admin);
//         if (it2 != invites.end())
//             invites.erase(it2);//if anythign goes wrong here, check the iterator
//     }
// }

// void Channel::remove_admin(Client *admin, std::string name) {
//     this->operate = false;
//     std::string reply_message;
//     Client *user = GetUser(name);

//     if (user) {
//         std::vector<Client*>::iterator it = std::find(admins.begin(), admins.end(), admin);

//         if (it != admins.end()){
//             admins.erase(it);
//             send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -o " + admin->getnickname()));
//         }
//     }
//     else {
//         reply_message = GetUserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->getnickname(), user->getnickname(), this->GetName());
//         sendMessage(reply_message, admin->get_clientfd());
//     }
// }

void Server::MODE( Client *client, std::stringstream &ss){
    std::string chan_name, mode, arg, reply_message;
    ss >> chan_name >> mode >> arg;
    Channel *chan = getChannel(chan_name);
    if(chan){
        if(!mode.empty()){
            if(chan->is_inChannel(client)){
                chan->valid_mode(client, mode, arg);
            }else{
                if(client)
                    reply_message = ":" + client->getIPaddress() + ERR_CHANOPRIVSNEEDED(client->getnickname(), chan_name);
            } 
        } else {
            if(!chan->get_topic().empty())
                chan->rpl_topic(client, "fda");
            chan->rpl_list(client);
            chan->rpl_mode(client);
        }
    }else{
            if(client)
            ssendMessage(":" + client->getIPaddress() + ERR_NOSUCHCHANNEL(client->getnickname(), chan_name), client->get_clientfd());
    }
}


void Server::KICK(Client *admin,  std::string &line)
{
    std::istringstream ss(line);
    std::string cmd, chan_name, nick, reason;
    ss >> cmd;
    ss >> chan_name;
    ss >> nick;
    std::getline(ss, reason);

    Channel *chan = getChannel(chan_name);
    Client  *clio = getClientname(nick);

    std::cout << " Ara wa7ed kick hna --#%& " << std::endl;
    std::string reply_message;

    if (chan)
    {
        if(clio)
        {
            if(chan->is_inChannel(clio))
            {
                if(chan->is_Admin(clio))
                {
                    chan->remove_admin(clio);
                }

                if(chan->is_Invited(clio))
                {
                    chan->remove_Invited(clio);
                } 
                std::string msg_to_reply = ":" + admin->getPrefix() + " KICK " + chan->GetName() + clio->getnickname() + " " + reason + "\r\n" ;
                chan->send_to_all(reply_message);
                chan->remove_user(clio);
            }
            else 
            {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOTONCHANNEL(nick, chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }


            if (chan->is_Admin(admin) && !chan->is_Admin(clio))
            {
                if (chan->is_inChannel(clio))
                {
                    if (chan->is_Invited(clio))
                        chan->remove_Invited(clio);
                    std::string msg_to_reply = ":" + admin->getPrefix() + " KICK " + chan_name +  " " + clio->getnickname() + " " + reason + "\r\n";
                    chan->send_to_all(msg_to_reply);
                    chan->remove_user(clio);
                } 
                else
                {
                    std::string msg_to_reply =  ":" + admin->getIPaddress() + ERR_NOTONCHANNEL(clio->getnickname(), chan_name);
                    ssendMessage( msg_to_reply , admin->get_clientfd());
                }
            }
            else
            {
                std::string msg_to_reply = ":" + admin->getIPaddress() + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan_name);
                ssendMessage( msg_to_reply  ,admin->get_clientfd() );
            }

        }
        else
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOTONCHANNEL(nick, chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
    }






    // if(!is_inChannel(admin)){
    //     reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
    //     sendMessage(reply_message, admin->get_clientfd());
    //     return;
    // }
    // //isalpha
    // if(!is_inChannel(user)){
    //     reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(user->getnickname(), this->GetName());
    //     sendMessage(reply_message, admin->get_clientfd());
    //     return;
    // }
    // send_to_all(GetUserInfo(user, true) + "clio " + this->GetName() + " " + user->getnickname() + " :" + (reason.empty() ? "bad content" : reason) + "\r\n");
    // remove_user(user);
}


void Server::INVITE(Client *admin, std::string &line){
    std::cout << " Ara wa7ed Invite hna --#%& " << std::endl;
    std::string reply_message;
    std::string cmd, nick , chan_name;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> nick;
    std::getline(ss, chan_name);

    Client *clio = getClientname(nick);
    Channel *chan = getChannel(chan_name);

    if (clio)
    {
        if (chan)
        {
            if(chan->is_Admin(admin))
            {
                if (chan->is_inChannel(admin))
                {
                    chan->invites.push_back(admin);
                    std::string msg_to_reply = admin->getIPaddress() + RPL_INVITING(admin , clio->getnickname() , chan_name);
                    ssendMessage(msg_to_reply, admin->get_clientfd());

                    std::string msg_to_reply = ":" + admin->getPrefix() + " INVITE " + clio->getnickname() + " " + chan->GetName() + "\r\n" ;
                    ssendMessage(msg_to_reply, clio->get_clientfd());
                }
                else
                {
                    std::string msg_to_reply = admin->getIPaddress() + ERR_USERONCHANNEL(admin , clio->getnickname(), chan_name);
                    ssendMessage(msg_to_reply, admin->get_clientfd());
                }
            }
            else
            {
                std::string msg_to_reply = ":" + admin->getIPaddress() + ERR_CHANOPRIVSNEEDED(admin, chan_name);
            }
        }
        else 
        {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        if (admin)
        {
            std::string msg_to_send = ":" + admin->getIPaddress() + ERR_NOSUCHNICK(admin->getnickname(), nick);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }


    // if(!is_inChannel(admin)){
    //     reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
    //     sendMessage(reply_message, admin->get_clientfd());
    //     return;
    // }
    // if(is_inChannel(user)){
    //     reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(user->getnickname(), this->GetName());////
    //     sendMessage(reply_message, admin->get_clientfd());
    //     return;
    // }
    // if (!is_inChannel(user))
    // sendMessage(GetUserInfo(admin, false) + RPL_INVITING(admin->getnickname(), user->getnickname(), this->GetName()), user->get_clientfd());
    // sendMessage(GetUserInfo(admin, false) + RPL_INVITING(admin->getnickname(), user->getnickname(), this->GetName()), admin->get_clientfd());
}

void Server::TOPIC(Client *admin, std::string &line)
{
    std::cout << " Ara wa7ed Topic hna --#%& " << std::endl;
    std::istringstream ss(line);
    std::string cmd , chan_name , topic;
    std::string reply_message;
    std::string error_message;

    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, topic);

    Channel *chan = getChannel(chan_name);
    if (chan)
    {
        if(topic.find(':') == std::string::npos)
        {
            if (chan->is_inChannel(admin))
            {
                if(chan->get_has_topic())
                {
                    if(!topic.empty()){
                        chan->SetTopic(topic);
                        reply_message = chan->GetUserInfo(admin, true) + RPL_TOPIC(admin->getnickname(), chan->GetName(), topic);
                        ssendMessage(reply_message, admin->get_clientfd());

                        reply_message = chan->GetUserInfo(admin, true) + RPL_TOPICWHOTIME(admin->getnickname(), chan->GetName(), admin->getnickname(), std::to_string(chan->get_topictime()));
                        ssendMessage(reply_message, admin->get_clientfd());
                    }
                }
                else
                {
                    reply_message = chan->GetUserInfo(admin, true) + RPL_NOTOPIC(admin->getnickname(), chan->GetName());
                    ssendMessage(reply_message, admin->get_clientfd());
                }
            }
            else 
            {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }
            return;
        }


        if(chan->is_Admin(admin))

            if (chan->get_modes().find('t') == std::string::npos)
            {
                chan->SetTopic(topic);
                reply_message = ":" + admin->getPrefix()   +  " TOPIC " + chan->GetName() + " " + topic + "\r\n";
                chan->send_to_all(reply_message);
            }
            else
            {
                   std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_INVALIDMODEPARAM(admin->getnickname(), chan_name, "TOPIC", topic, "Channel Topic Restrection Are On");
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }

        else
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        if (admin)
        {
             std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}


//         // if(!is_inChannel(admin)){
//         //     error_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
//         //     sendMessage(error_message, admin->get_clientfd());
//         //     return;
//         // }
//         // if(has_topic){
//         // }
//         // else{
//         //     if(!topic_message.empty()){
//         //         SetTopic(topic_message);
//         //         reply_message = GetUserInfo(admin, true) + RPL_TOPIC(admin->getnickname(), this->GetName(), topic_message);
//         //         sendMessage(reply_message, admin->get_clientfd());
//         //         return;
//         //     }
//         //     else{
//         //         reply_message = GetUserInfo(admin, false) + RPL_NOTOPIC(admin->getnickname(), this->GetName());
//         //         sendMessage(reply_message, admin->get_clientfd());
//         //         return;
        
//         // }
//     }
// }

void Server::PART(Client *admin, std::string &line)
{
    std::cout << " Ara wa7ed Part hna --#%& " << std::endl;
    std::string cmd , chan_name , reason;

    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, reason);
    Channel *chan = getChannel(chan_name);
    std::string reply_message;

    if (chan)
    {
        if (chan->is_inChannel(admin))
        {

            if(chan->is_Admin(admin))
                chan->remove_admin(admin);
            if(admin)
            {
                reply_message = ":" + admin->getPrefix() + " PART " + chan->GetName() + " :leaving channel\r\n";
                chan->send_to_all(reply_message);
                chan->remove_user(admin);
            }
        }
        else
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }

    }
    else
    {
        if (admin)
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}
// // Special Considerations: !!!!!!!!!!!
// // If the last operator leaves a channel (e.g., by using PART), the channel may be left without any operator unless another user is automatically promoted to operator. In some IRC implementations, this happens to ensure channel continuity.
// // An operator leaving via PART does not override channel restrictions like invite-only mode (+i) or password protection (+k). Rejoining the channel would still require them to adhere to those rules.
// /////////////////////////////////////


bool Channel::onChannel(Client *admin)
{
    if(std::find(Clients.begin(), Clients.end(), admin) != Clients.end())
        return true;
    return false;
}

void Server::PRIVMSG(Client *admin, std::string &line)
{   
    std::string cmd , nicknameiichannel , msg;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> nicknameiichannel;
    std::getline(ss, msg);

    std::cout << " Ara wa7ed Privmsg hna --#%& " << std::endl;
    if (nicknameiichannel[0] == '#')
    {
        Channel *chan = getChannel(nicknameiichannel);
        if (chan)
        {
            if (!chan->onChannel(admin))
            {
                std::string error_msg = ERR_NOTONCHANNEL(admin->getnickname(), chan->GetName());
                send(admin->get_clientfd(), error_msg.c_str(), error_msg.length(), 0);
            }

            if(msg[0] == ':')
            {
                std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getIPaddress() + " PRIVMSG " + chan->GetName() + " :" + msg + "\r\n";
                chan->send_to_all(msg_to_send);
            }
        }
        else
        {
            if (admin)
            {
                // _server->getIpaddress() is the same as admin->getIPaddress()
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), nicknameiichannel);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }

        }
    }
    else 
    {
        Client *clio = getClientname(nicknameiichannel);
        if (clio)
        {
            std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getIPaddress() + " PRIVMSG " +  clio->getnickname() + " :" + msg + "\r\n";
            std::string	er = ("Cannot send message: client is not active or socket is invalid");
            if (clio->get_clientfd() <= 0)
            {
                ssendMessage(er, admin->get_clientfd());
            }
            else 
            {
                ssendMessage(msg_to_send , clio->get_clientfd());
            }
        }
        else 
        {
            if (admin)
            {
                // _server->getIpaddress()
                std::string msg_to_send = ":" + admin->getIPaddress() + ERR_NOSUCHNICK(admin->getnickname(), nicknameiichannel);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }
        }
    }
    
        // for (size_t i = 0; i < Clients.size(); ++i) {
        //     if (Clients[i]->getnickname() == admin->getnickname()) {
        //         continue;
        //     }
        //     std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@localhost PRIVMSG " + chan->GetName() + " :" + message + "\r\n";
        //     send(Clients[i]->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        // }
        // return 0;
    // }

    // if (target == NULL || target->getnickname().empty()){
    //     std::string error_msg = ERR_NOSUCHNICK(admin->getnickname(), "unknown");
    //     send(admin->get_clientfd(), error_msg.c_str(), error_msg.length(), 0);
    //     return -1;
    // }

    // std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@localhost PRIVMSG " + target->getnickname() + " :" + message + "\r\n";
    // send(target->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
    // return 0;
}

// // >> :luna.AfterNET.Org 352 mboudrio #some lo 88ABE6.25BF1D.D03F86.88C9BD.IP *.afternet.org mboudrio H@x :0 realname
// // >> :luna.AfterNET.Org 315 mboudrio #some :End of /WHO list
// // >> :127.0.0.1  315 mboudrio #some :End of /WHO list
// //  :10.11.2.5 315 mboudrio #some End of /WHOIS list
// //  :127.0.0.1 315 mboudrio #some :End of /WHOIS list

// // :10.11.2.5 352 mboudrio #some lo 127.0.0.1 mboudrio H@ :0 realname
// // :127.0.0.1 352 mboudrio #ch lo 127.0.0.1 mboudrio H@:0 realname


void Server::WHO(Client* admin, std::string &line)
{
    std::string chan_name , cmd;

    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;

    std::string reply_message;
    Channel *chan = getChannel(chan_name);

    if (!chan)
    {
        if(admin)
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}
    // for(size_t i = 0; i < Clients.size(); ++i){
    //     std::cout << "count == " << i + 1 << "\n\n";
    //     if(is_Admin(Clients[i]))
    //         reply_message = ":" + admin->getservername() + " 352 " + admin->getnickname() + " " + this->GetName() + " " + Clients[i]->getusername() + " " + Clients[i]->getIPaddress() + " " + Clients[i]->getnickname() + " H@" + " :0 realname\r\n";
    //     else
    //         reply_message = ":" + admin->getservername() + " 352 " + admin->getnickname() + " " + this->GetName() + " " + Clients[i]->getusername() + " " + Clients[i]->getIPaddress() + " " + Clients[i]->getnickname() + " G" + " :0 realname\r\n";
    //     sendMessage(reply_message, admin->get_clientfd());//if something goes wrong here, 7et sendMessage f loop   
    // }
    // sendMessage(GetUserInfo(admin, false) + RPL_ENDOFWHOIS(admin->getnickname(), this->GetName()), admin->get_clientfd());

