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
#include<thread>
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

class Conversation{
    public: //private;
        int user_socket;
        int user_listen_socket;
        std::list<int> connected_sockets;
        int conv_port;
        std::list<pthread_t*> threads;

        void send_message(std::string message);

    public:
        bool is_connected;

        Conversation(int port);
        ~Conversation();

        int receive_message(int socket_id);
        void listen_to_user();
};
