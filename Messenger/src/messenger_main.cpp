#include "messenger_tools.cpp"

int main(int argc, char* argv[]){
    cout<<"Console instant Messenger Application"<<endl;
    cout<<"type \"close\" to close it"<<endl;

    string buff;
    pthread_t thread;

    if(conv.is_connected == false){
        cout<<"Couldn't connect to server"<<endl;
        getchar();
        return 0;
    }
    pthread_create(&thread, NULL, &listen_thread, NULL);
    while(1){
        buff = "";
        getline(cin, buff);
        if(buff == "close") break;
        //conv.send_message(buff);

    }
    cout<<"Closing messenger..."<<endl;
    pthread_cancel(thread);
    cout<<"click to continue"<<endl;
    getchar();
    return 0;
}
