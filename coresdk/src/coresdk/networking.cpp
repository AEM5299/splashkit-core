#include <sstream>
#include <cmath>
#include <iomanip>
#include <backend/network_driver.h>
// FIXME Fix this bad import
#include <backend/network_driver.cpp>
#include "networking.h"

namespace splashkit_lib
{
    sk_server_socket& create_server(const string &name, unsigned short int port)
    {
        return create_server(name, port, TCP);
    }

    // TODO this should return &sk_server_socket i think
    sk_server_socket& create_server(const string &name, unsigned short int port, sk_connection_type protocol)
    {
        sk_network_connection con = protocol == UDP ? sk_open_udp_connection(port)
                                            : sk_open_tcp_connection(nullptr, port);

        sk_server_socket* socket = new sk_server_socket;
        if (con._socket && (con.kind == SGCK_TCP || con.kind == SGCK_UDP))
        {
            // TODO create on heap and change server_sockets to be map<string, sk_server_socket*>

            // TODO initialise fields
            socket->id = SERVER_SOCKET_PTR;
            socket->name = name;
            socket->port = port;
            socket->socket = con;
            socket->protocol = protocol;
            socket->newConnections = 0;

            server_sockets.insert({name, *socket});
        }
        return (sk_server_socket&)socket;
    }

    sk_server_socket server_named(const string &name)
    {
        return server_sockets[name];
    }

    bool close_server(const string &name)
    {
        return close_server(server_sockets[name]);
    }

    bool close_server(sk_server_socket &svr)
    {
        // Ref cannot be null, therefore this always returns true
        //if (svr != null) return false;

        for(auto const &connection : svr.connections)
        {
            if (!close_connection(connection)) return false;
        }

        server_sockets.erase(svr.name);

        delete svr;

        return true;
    }

    void close_all_servers()
    {
        for(auto const &pair : server_sockets)
        {
            close_server(pair.second);
        }
    }

    bool server_has_new_connection(const string &name)
    {
        return server_has_new_connection(server_sockets[name]);
    }

    bool server_has_new_connection(sk_server_socket& server)
    {
        return server.newConnections > 0;
    }

    bool has_new_connections()
    {
        for(auto const &pair : server_sockets)
        {
            if (server_has_new_connection(pair.second))
            {
                return true;
            }
        }
        return false;
    }

    sk_connection open_connection(const string &host, unsigned short int port)
    {
        return open_connection(name_for_connection(host, port), host, port, TCP);
    }

    sk_connection open_connection(const string &name, const string &host, unsigned short int port)
    {
        return open_connection(name, host, port, TCP);
    }

    sk_connection open_connection(const string &name, const string &host, unsigned short int port, sk_connection_type protocol)
    {
        return sk_connection();
    }

    sk_connection retrieve_connection(const string &name, int idx)
    {
        return retrieve_connection(server_sockets[name], idx);
    }

    sk_connection retrieve_connection(sk_server_socket server, int idx)
    {
        return server.connections.size() > idx ? server.connections[idx] : nullptr;
    }

    void close_all_connections()
    {
        for(auto const &pair : connections)
        {
            close_connection(pair.second);
        }
    }

    bool close_connection(sk_connection &a_connection)
    {
        //clear_messages(a_connection);
        //shut_connection(a_connection);
        // remove connection from connections map
        // remove connection from servers map
        return false;
    }

    bool close_connection(const string &name)
    {
        return close_connection(connections[name]);
    }

    int connection_count(const string &name)
    {
        return connection_count(server_sockets[name]);
    }

    int connection_count(sk_server_socket server)
    {
        return server.connections.size();
    }

    unsigned int connection_ip(const string &name)
    {
        return connection_ip(connections[name]);
    }

    unsigned int connection_ip(sk_connection a_connection)
    {
        return a_connection.ip;
    }

    sk_connection connection_named(const string &name)
    {
        return sk_connection();
    }

    bool is_connection_open(sk_connection con)
    {
        return con.open;
    }

    bool is_connection_open(const string &name)
    {
        return connection_open(connections[name]);
    }

    unsigned short int connection_port(sk_connection a_connection)
    {
        return a_connection.port;
    }

    unsigned short int connection_port(const string &name)
    {
        return connection_port(connections[name]);
    }

    sk_connection last_connection(sk_server_socket server)
    {
        if (server.connections.empty())
        {
            return nullptr;
        }
        return server.connections[server.connections.size()-1];
    }

    sk_connection last_connection(const string &name)
    {
        return last_connection(server_sockets[name]);
    }

    void reconnect(const string &name)
    {
        reconnect(connections[name]);
    }

    void reconnect(sk_connection a_connection)
    {
        string host = a_connection.stringIP;
        unsigned short port = a_connection.port;

        sk_close_connection(&a_connection.socket);
        //a_connection.open = establish_connection(, host, port, a_connection.protocol)
    }

    sk_connection message_connection(sk_message msg)
    {
        return msg.connection;
    }

    void broadcast_message(const string &a_msg)
    {
        for(auto const& tcp_server: server_sockets)
        {
            broadcast_message(a_msg, tcp_server);
        }
        for (auto const& udp_connection: connections)
        {
            broadcast_message(a_msg, udp_connection);
        }
    }

    void broadcast_message(const string &a_msg, const string &name)
    {
        broadcast_message(a_msg, server_sockets[name]);
    }

    void broadcast_message(const string &a_msg, sk_server_socket svr)
    {
        for (auto const& tcp_connection: svr.connections)
        {
            broadcast_message(a_msg, tcp_connection);
        }
    }

    void check_network_activity()
    {

    }

    void clear_messages(sk_server_socket svr)
    {
        // TODO delete all messages
    }

    void clear_messages(sk_connection a_connection)
    {
        // TODO delete all messages
    }

    void clear_messages(const string &name)
    {
        if (server_sockets.count(name))
        {
            clear_messages(server_sockets[name]);
        }
        else if (connections.count(name))
        {
            clear_messages(connections[name]);
        }
    }

    void free_message(sk_message msg)
    {
        // TODO Find swingame equivalent
    }

    bool has_messages()
    {
        for(auto const& tcp_server: server_sockets)
        {
            if (has_messages(tcp_server))
            {
                return true;
            }
        }
        for (auto const& udp_connection: connections)
        {
            if (has_messages(udp_connection))
            {
                return true;
            }
        }
        return false;
    }

    bool has_messages(sk_connection con)
    {
        return con.messages.empty();
    }

    bool has_messages(sk_server_socket svr)
    {
        return svr.messages.empty();
    }

    bool has_messages(const string &name)
    {
        return connections.count(name) ? connections[name].messages.empty()
                                       : server_sockets[name].messages.empty();
    }

    int message_count(sk_connection a_connection)
    {
        return a_connection.messages.size();
    }

    int message_count(const string &name)
    {
        return connections.count(name) ? connections[name].messages.size()
                                       : server_sockets[name].messages.size();
    }

    int message_count(sk_server_socket svr)
    {
        return svr.messages.size();
    }

    string message_data(sk_message msg)
    {
        return msg.data;
    }

    string message_host(sk_message msg)
    {
        return msg.host;
    }

    unsigned short int message_port(sk_message msg)
    {
        return msg.port;
    }

    sk_connection_type message_protocol(sk_message msg)
    {
        return msg.protocol;
    }

    sk_message read_message(sk_connection a_connection)
    {
        return sk_message();
    }

    sk_message read_message(const string &name)
    {
        return sk_message();
    }

    sk_message read_message(sk_server_socket svr)
    {
        return sk_message();
    }

    string read_message_data(sk_connection a_connection)
    {
        return std::__cxx11::string();
    }

    string read_message_data(sk_server_socket svr)
    {
        return std::__cxx11::string();
    }

    string read_message_data(const string &name)
    {
        return std::__cxx11::string();
    }

    bool send_message_to(const string &a_msg, sk_connection a_connection)
    {
        return false;
    }

    bool send_message_to(const string &a_msg, const string &name)
    {
        return false;
    }

    string name_for_connection(const string host, const unsigned int port)
    {
        stringstream str;
        str << host << ":" << port;
        return str.str();
    }

    string hex_str_to_ipv4(const string &a_hex)
    {
        stringstream ipv4_string;
        ipv4_string << hex_to_dec_string(a_hex.substr(2,2));
        ipv4_string << "." << hex_to_dec_string(a_hex.substr(4,2));
        ipv4_string << "."<< hex_to_dec_string(a_hex.substr(6,2));
        ipv4_string << "." << hex_to_dec_string(a_hex.substr(8,2));
        return ipv4_string.str();
    }

    string hex_to_dec_string(const string &a_hex)
    {
        int dec = 0;
        for (int i = 0; i < a_hex.length(); i++)
        {
            int c_val = 0;
            if (a_hex[i] - '0' < 10)
            {
                c_val = a_hex[i] - '0';
            }
            else if (a_hex[i] - 'A' < 6)
            {
                c_val = (a_hex[i] - 'A') + 10;
            }
            dec += c_val * pow(16, (a_hex.length() - i - 1));
        }
        return to_string(dec);
    }

    string dec_to_hex(unsigned int a_dec)
    {
        uint32_t dec = (uint32_t) a_dec;
        stringstream hex_string;
        hex_string << "0x" << uppercase << hex << dec;
        return hex_string.str();
    }

    unsigned int ipv4_to_dec(const string &a_ip)
    {
        string::size_type lastpos = 0;
        unsigned int result = 0;
        for(unsigned int i = 0; i < 4; i++)
        {
            string::size_type pos = a_ip.find('.', lastpos);
            string token = pos == -1 ? a_ip.substr(lastpos) : a_ip.substr(lastpos, pos - lastpos);

            result += (token == "" || (lastpos == 0 && i > 0) ? 0 : stoi(token) << (3 - i) * 8);
            lastpos = pos + 1;
        }

        return result;
    }

    string ipv4_to_hex(const string& a_ip)
    {
        string::size_type lastpos = 0;
        stringstream hex_string;
        hex_string << "0x";
        for(unsigned int i = 0; i < 4; i++)
        {
            string::size_type pos = a_ip.find('.', lastpos);
            string token = pos == -1 ? a_ip.substr(lastpos) : a_ip.substr(lastpos, pos - lastpos);

            hex_string << setw(2) << setfill('0') << uppercase << hex
            << (token == "" || (lastpos == 0 && i > 0) ? 0 : stoi(token));

            lastpos = pos + 1;
        }

        return hex_string.str();
    }

    string ipv4_to_str(unsigned int ip)
    {
        uint32_t ipaddr = (uint32_t) ip;
        stringstream ip_string;
        ip_string << ((ipaddr >> 24) & 0xFF) << ".";
        ip_string << ((ipaddr >> 16) & 0xFF) << ".";
        ip_string << ((ipaddr >> 8) & 0xFF) << ".";
        ip_string << (ipaddr & 0xFF);
        return ip_string.str();
    }
    
    string my_ip()
    {
        // TODO implement ip address resolution. Should return ip address of connected network if one exists.
        return "127.0.0.1";
    }
}