#include "user_tools.cpp"

int main(int arg, char* argv[]){
    if(arg < 3){
        cout<<"Usage: ./user <nickname> <conversation port>"<<endl;
        return 1;
    }
    cout<<"Connecting to Messenger"<<endl;
    cout<<"To send message to all conversation members, type it and press ENTER"<<endl;
    cout<<"To send file, type \"SEND FILE file_name\" and press ENTER"<<endl;
    cout<<"To quit the conversation, type \"quit\" and press ENTER"<<endl;

    string buff;
    pthread_t thread1, thread2;
    ifstream ip_file;
    string conversation_address = "0.0.0.0";

    ip_file.open("conv_ip.ini", ifstream::in);
    while(ip_file.good()){
        getline(ip_file, buff);
        if(!ip_file.eof() && buff[0] != '*') {
            conversation_address = buff;
            break;
        }
    }
    me.run(conversation_address, atoi(argv[2]));
    me.set_name(argv[1]);
    if(me.is_connected == false){
        cout<<"ERROR Couldn't connect to this conversation"<<endl;
        getchar();
        return 0;
    }
    pthread_create(&thread1, NULL, &receive_thread, NULL);
    pthread_create(&thread2, NULL, &send_files_thread, NULL);
    while(1){
        buff = "";
        getline(cin, buff);
        if(buff == "quit") {
            me.send_message("left the conversation");
            break;
        }
        if(me.send_message(buff)){
            cout<<"no siema"<<endl;
            break;
        }
    }
    cout<<"Leaving conversation..."<<endl;
    pthread_cancel(thread1);
    pthread_cancel(thread2);
    getchar();
    return 0;
}
