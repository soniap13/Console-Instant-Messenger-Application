#include "user_tools.cpp"

int main(int arg, char* argv[]){
    if(arg < 3){
        cout<<"Usage: ./user <nickname> <conversation port>"<<endl;
        return 1;
    }
    me.set_name(argv[1]);
    string buff;
    cout<<"Connecting to Messenger"<<endl;
    cout<<"To send message to all conversation members, type it and press ENTER"<<endl;
    cout<<"To send file, type \"SEND FILE file_name\" and press ENTER"<<endl;
    cout<<"To quit the conversation, type \"quit\" and press ENTER"<<endl;
    string conversation_address = "178.43.72.211";
    //string conversation_address = "192.168.1.16";
    me.run(conversation_address, atoi(argv[2]));
    if(me.is_connected == false){
        cout<<"ERROR Couldn't connect to this conversation"<<endl;
        getchar();
        return 0;
    }
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, &receive_thread, NULL);
    pthread_create(&thread2, NULL, &send_files_thread, NULL);
    while(1){
        buff = "";
        getline(cin, buff);
        if(buff == "quit") break;
        if(me.send_message(buff)){
            cout<<"ERROR: Couldn't send the message"<<endl;
            break;
        }
    }
    me.send_message("left the conversation");
    cout<<"Leaving conversation..."<<endl;
    pthread_cancel(thread1);
    pthread_cancel(thread2);
    getchar();
    return 0;
}
