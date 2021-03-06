#include "user.hpp"
using namespace std;

User me;

User::User(){
    is_connected = false;
}

void User::run(string ip, int port){
    ip_address = ip;
    conv_port = port;
    unsigned int addr;
    hostent *host;
    sockaddr_in conversation;

    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id < 0){
        perror("User::run - socket()");
        return;
    }
    addr = inet_addr(ip_address.c_str());
    if(addr = 0){
        cout<<"Invalid ip address "<<ip_address<<endl;
        return;
    }
    host = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    if(host = nullptr){
        perror("User::run - gethostbyaddr()");
        return;
    }
    conversation.sin_addr.s_addr = addr;
    conversation.sin_family = AF_INET;
    conversation.sin_port = htons(conv_port);
    if(connect(socket_id, (sockaddr*)&conversation, sizeof(conversation))){
        perror("User::run - connect()");
        close(socket_id);
        return;
    }
    is_connected = true;
}

User::~User(){
    if(is_connected) close(socket_id);
}

int User::send_message(string message){
    if(message.find("SEND FILE ") == 0){ //receiving request for sending file
        if(message.length() > 10){
            files_to_send.push(message.substr(10)); //adds file to the queue
            cout<<"adding "<<message.substr(10)<<" to files to send"<<endl;
        }
        else
            cout<<"Specify file name"<<endl;
    }
    else {
        if(message.find("HEADER BEGIN") != 0)
            message = "HEADER BEGIN name=" + name + " type=MESSAGE END " + message;
        int m_size = message.length() + 1;
        char m[m_size];
        strcpy(m, message.c_str());
        if(send(socket_id, m, m_size, 0) < 0){
            return 1;
        }
    }
    return 0;
}

void User::send_file(){
    if(files_to_send.size() == 0) return;

    off_t offset = 0;
    struct stat stat_buf;
    int rc, fd, j = 0;

    string file_name = files_to_send.front();
    fd = open(file_name.c_str(), O_RDONLY);
    if(fd < 1){
        cout<<"Couldn't open file "<<file_name<<": "<<strerror(errno)<<endl;
        cout<<"Check file name/path/directory and try again"<<endl;
        files_to_send.pop();
        return;
    }
    fstat(fd, &stat_buf);
    long file_size = stat_buf.st_size;
    while(j < 3){ //tryying to send file to all connected user, but only trying 3 times
        me.send_message("HEADER BEGIN name=" + me.name + " type=FILE file_name="
        + file_name + " file_size=" + to_string(file_size) + " END");
        cout<<"sending file "<<file_name<<endl;
        rc = sendfile(socket_id, fd, &offset, stat_buf.st_size);
        if(fd != -1 && rc == stat_buf.st_size){ //succsefully sent
            break;
        }
        j++;
    }
    if(j == 3){
        cout<<"Couldn't send file "<<file_name<<". Maybe try again."<<endl;
        cout<<"ERROR: "<<strerror(errno)<<endl;
    }
    files_to_send.pop();
}

int get_name_type(const string &message, string &name, string &type){
    if(message.find("HEADER BEGIN ") != 0) return 2;
    int end = message.find("END");
    if(end == string::npos) return 1;
    int n = message.find("name=");
    if(n == string::npos || n > end) name = "user";
    else {
        name = message.substr(n+5);
        name = name.substr(0, name.find(" "));
    }
    int t = message.find("type=");
    if(t == string::npos || t > end) return 1;
    else {
        type = message.substr(t+5);
        type = type.substr(0, type.find(" "));
    }
    return 0;
}

int get_filename_size(const string &message, string &file_name, int &file_size){
    int end = message.find("END");
    int n = message.find("file_name=");
    string size_str;
    if(n == string::npos || n > end) return 1;
    else {
        file_name = message.substr(n+10);
        file_name = file_name.substr(0, file_name.find(" "));
    }
    int s = message.find("file_size=");
    if(s == string::npos || s > end) return 1;
    else {
        size_str = message.substr(s + 10);
        file_size = stoi(size_str.substr(0, size_str.find(" ")));
    }
    return 0;
}

char * get_message(char * buff){
    return strstr(buff, "END ") + 4;
}

int User::receive_message(){
    char buff[1024];
    ofstream received_file;
    string type, nick, message;
    static string file_name;
    static int file_size, size_received;
    int rc = recv(socket_id, buff, 1024, 0);

    if(rc <= 0){
        cout<<"ERROR: conversation was closed"<<endl;
        is_connected = false;
        return 1;
    }
    if(get_name_type(buff, nick, type) == 1){
        cout<<"ERROR: wrong header in message"<<endl;
        return 0;
    }
    else if(type == "FILE"){ //received file header
        get_filename_size(buff, file_name, file_size);
        cout<<"file name - "<<file_name<<endl;
        size_received = 0;
    }
    else if(type == "MESSAGE"){ //received normal message
        cout<<nick<<": "<<get_message(buff)<<endl;
    }
    else{ // received bare file
        received_file.open(file_name.c_str());
        cout<<"opening file "<<file_name<<endl;
        cout<<"writing "<<buff<<" do pliku"<<endl;
        size_received = size_received + rc;
        received_file<<buff;
        received_file.close();
    }
    return 0;
}

void * receive_thread(void * p){
    while(1)
        if(me.receive_message()) break;
}

void * send_files_thread(void * p){
    while(me.is_connected)
        me.send_file();
}

void User::set_name(string n){
    name = n;
}


