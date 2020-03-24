#include "messenger.hpp"
using namespace std;

Conversation conv(8084);

void * receive_thread(void* socket_id){
    while(1)
        if(conv.receive_message((intptr_t)socket_id)) {
            cout<<"konczymy watek dla "<<socket_id<<endl;
            break;
        }
}

void * listen_thread(void * p){
    while(1)
        conv.listen_to_user();
}

Conversation::Conversation(int port){
    is_connected = false;
    sockaddr_in recipient;

    recipient.sin_family = AF_INET;
    recipient.sin_addr.s_addr = INADDR_ANY;
    cout<<"Runnig on address "<<recipient.sin_addr.s_addr<<endl;
    recipient.sin_port = htons((u_short)port);

    user_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(user_listen_socket < 0){
        perror("Conversation::Conversation - socket()");
        exit(1);
    }
    if(bind(user_listen_socket, (sockaddr*)&recipient, sizeof(recipient)) != 0){
        perror("Conversation::Conversation - bind()");
        exit(1);
    }
    if(listen(user_listen_socket, 10) != 0 ){
        perror("Conversation::Conversation - listen()");
        exit(1);
    }
    is_connected = true;
}

Conversation::~Conversation(){
    for(pthread_t * t : threads) pthread_cancel(*t);
    close(user_listen_socket);
}

void Conversation::listen_to_user(){

    sockaddr_in sender;
    int sender_len = sizeof(sender);
    int new_socket = accept(user_listen_socket, (struct sockaddr*)&sender, (socklen_t*)&sender_len);
    if(new_socket < 0){
        perror("Conversation::listen_to_user - accept()");
        exit(1);
    }
    cout<<"new user connected "<<new_socket<<endl;
    connected_sockets.emplace_back(new_socket);
    pthread_t *thread = new pthread_t;
    intptr_t x = (intptr_t)new_socket;
    pthread_create(thread, NULL, &receive_thread, (void*)x);
    threads.emplace_back(thread);
}

int Conversation::send_message(string message){
    if(connected_sockets.size() == 0) return 0;
    list<int>::iterator i;
    int s;

    for(i = connected_sockets.begin(); i != connected_sockets.end(); i++){
        s = send(*i, message.c_str(), message.size() + 1, 0);
        if(s < 0){
            cout<<"user "<<*i<<" leaving convesation"<<endl;
            connected_sockets.remove(*i);
        }
    }
    //(s < 0) return 1; //couldnt send message to last socket
    return 0;
}

/*
int Conversation::send_file(){
    if(connected_sockets.size() == 0) return 0;
    if(files_to_send.size() == 0) return 0;

    off_t offset = 0;
    struct stat stat_buf;
    int rc, fd, j = 0;
    list<int>::iterator i;

    string file_name = files_to_send.front();
    fd = open(file_name.c_str(), O_RDONLY);
    if(fd < 0){
        cout<<"Couldn't open file "<<file_name<<": "<<strerror(errno)<<endl;
        cout<<"Check file name/path/directory and try again"<<endl;
        files_to_send.pop();
        return 0;
    }
    fstat(fd, &stat_buf);
    list<int> users_to_send_file(connected_sockets);
    while(users_to_send_file.size() != 0 && j < 3){ //tryying to send file to all connected user, but only trying 3 times
        for(i = users_to_send_file.begin(); i != users_to_send_file.end(); i++){
            rc = sendfile(*i, fd, &offset, stat_buf.st_size);
            if(fd != -1 && rc == stat_buf.st_size){ //succsefully sent
                users_to_send_file.remove(*i);
            }
        }
        j++;
    }
    if(users_to_send_file.size() != 0){
        cout<<"Couldn't send file "<<file_name<<" to all users. Maybe try again."<<endl;
    }
    files_to_send.pop();
    return 0;
}
*/
int Conversation::receive_message(int socket_id){
    int rc, index;
    cout<<"receiving message from "<<socket_id<<endl;
    int buff_size = 1024;
    char buff[buff_size];
    if(recv(socket_id, buff, buff_size, 0) <= 0){
        connected_sockets.remove(socket_id);
        cout<<"socket "<<socket_id<<" no longer available"<<endl;
        return 1;
    }
    else{
        cout<<buff<<endl;
        send_message(buff);
        return 0;
    }
    return 0;
}

void Conversation::print_info(){
    cout<<"Conversation with sockets: ";
    for(int s : connected_sockets) cout<<s<<" ";
    cout<<endl;
}

int main(int argc, char* argv[]){
    string buff;
    cout<<"Console instant Messenger Application"<<endl;
    cout<<"type \"close\" to close it"<<endl;

    if(conv.is_connected == false){
        cout<<"Couldn't connect to server"<<endl;
        getchar();
        return 1;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, &listen_thread, NULL);
    while(1){
        buff = "";
        getline(cin, buff);
        if(buff == "close") break;
        /*
        if(conv.send_message(buff)){
            cout<<"Problem with connecting to server"<<endl;
            break;
        }
        */
    }
    cout<<"Closing messenger..."<<endl;
    pthread_cancel(thread);
    cout<<"click to continue"<<endl;
    getchar();
    return 0;
}
