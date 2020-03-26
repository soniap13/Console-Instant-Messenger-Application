#include<iostream>
#include<list>
#include<algorithm>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<cstdlib>
#include<errno.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<pthread.h>
#include<cstdint>
#include<cstring>
#include<stdio.h>
#include<netdb.h>
#include<queue>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fstream>

class User{
    public:
        std::string name;
        bool is_connected;
        std::string ip_address;
        int socket_id;
        int conv_port;
        std::queue<std::string> files_to_send;

        User();
        ~User();
        friend class Server;
        friend class Conversation;

        void run(std::string ip_address, int port);
        int send_message(std::string message);
        int send_file();
        int receive_message();
        void set_name(std::string n);
};
