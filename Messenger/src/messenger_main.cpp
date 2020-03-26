#include "messenger_tools.cpp"

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
