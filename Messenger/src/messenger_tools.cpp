#include "messenger.hpp"
using namespace std;

Conversation conv(8084);

void * receive_thread(void* socket_id){
    while(1)
        if(conv.receive_message((intptr_t)socket_id)) {
            break;
        }
}

void * listen_thread(void * p){
    while(1)
        conv.listen_to_user();
}

Conversation::Conversation(int port){
    is_connected = true;
    conv_port = port;
    sockaddr_in recipient;

    recipient.sin_family = AF_INET;
    recipient.sin_addr.s_addr = INADDR_ANY;
    recipient.sin_port = htons((u_short)port);
    user_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(user_listen_socket < 0){
        perror("Conversation::Conversation - socket()");
        is_connected = false;
    }
    else if(bind(user_listen_socket, (sockaddr*)&recipient, sizeof(recipient)) != 0){
        perror("Conversation::Conversation - bind()");
        is_connected = false;
    }
    else if(listen(user_listen_socket, 10) != 0 ){
        perror("Conversation::Conversation - listen()");
        is_connected = false;
    }
}

Conversation::~Conversation(){
    for(pthread_t * t : threads) pthread_cancel(*t);
    close(user_listen_socket);
}

void Conversation::listen_to_user(){
    sockaddr_in sender;
    int sender_len = sizeof(sender);
    pthread_t *thread;
    intptr_t x;

    int new_socket = accept(user_listen_socket, (sockaddr*)&sender, (socklen_t*)&sender_len);
    if(new_socket < 0){
        perror("Conversation::listen_to_user - accept()");
        exit(1);
    }
    cout<<"new user connected "<<new_socket<<endl;
    connected_sockets.emplace_back(new_socket);
    thread = new pthread_t;
    x = (intptr_t)new_socket;
    pthread_create(thread, NULL, &receive_thread, (void*)x);
    threads.emplace_back(thread);
}

void Conversation::send_message(string message){
    list<int>::iterator i;
    int s;

    if(connected_sockets.size() == 0) return;
    for(i = connected_sockets.begin(); i != connected_sockets.end(); i++){
        s = send(*i, message.c_str(), message.size() + 1, 0);
        if(s < 0){
            cout<<"user "<<*i<<" leaving convesation"<<endl;
            connected_sockets.remove(*i);
        }
    }
}

int Conversation::receive_message(int socket_id){
    int rc, index;
    int buff_size = 1024;
    char buff[buff_size];

    if(recv(socket_id, buff, buff_size, 0) <= 0){
        connected_sockets.remove(socket_id);
        cout<<"socket "<<socket_id<<" no longer available"<<endl;
        return 1;
    }
    else{
        send_message(buff);
        return 0;
    }
    return 0;
}
