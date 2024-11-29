#include "Parcing_data.hpp"

Buffer::Buffer()
{
}
Buffer::~Buffer()
{}

// getters 

std::string Buffer::get_arg()
{
    return this->Arg;
}
std::string Buffer::get_cmd()
{
    return this->Cmd;
}
std::string Buffer::get_msg()
{
    return this->Msg;
}

std::string Buffer::get_trg()
{
    return this->trg;
}

std::string Buffer::get_prv_msg()
{
    return get_msg() + this->trg;
}

std::string  Buffer::get_reason()
{
    return get_arg() + get_msg() + get_trg();
}

Buffer::Buffer(const Buffer& newBuffer)
{
    *this = newBuffer;
}

Buffer &Buffer::operator=(Buffer const &newBuffer)
{
    if (this != &newBuffer)
    {
        this->Cmd = newBuffer.Cmd;
        this->Arg = newBuffer.Arg;
        this->Msg = newBuffer.Msg;
        this->trg = newBuffer.trg;
        this->prv_msg = newBuffer.prv_msg;
        this->reason = newBuffer.reason;
    }
    return *this;
}

void Buffer::Parcing_core(std::string &buffer)
{
    split_buffer_from_nc(buffer);
    print_parsed_data();
}

void Buffer::print_parsed_data() 
{
        std::cout << "Command: " << Cmd << "\n"
                  << "Argument: " << Arg << "\n"
                  << "Message: " << Msg << "\n"
                  << "target: " << trg << "\n"; 
}

void Buffer::split_buffer_from_nc(const std::string &buffer)
{
    std::istringstream ss(buffer);
    buf = buffer;
    ss >> Cmd;
    trim(Cmd);                
    ss >> Arg;
    trim(Arg);
    ss >> Msg;
    trim(Msg);                
    std::getline(ss, trg); 
    trim(trg);             
}

void Buffer::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

std::string Buffer::get_target()
{
    size_t pos = this->get_cmd().find(" ");
    if(pos == std::string::npos)
        return "";
    std::string target = this->get_cmd().substr(pos + 1);
    size_t pos2 = target.find(" :");
    if(pos2 != std::string::npos)
        target = target.substr(0, pos2);
    return target;
}