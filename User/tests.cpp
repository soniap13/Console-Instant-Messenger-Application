#include "src/user_tools.cpp"
#include<gtest/gtest.h>

User u;
User u2;
string ip = "123.45.67.89";
int port = 8882;
string name = "Name";

TEST(User, Constructor){
    EXPECT_TRUE(u.is_connected == false);
}

TEST(User, run){
    u.run(ip, port);
    EXPECT_EQ(port, u.conv_port);
    EXPECT_EQ(ip, u.ip_address);
    EXPECT_TRUE(u.socket_id >= 0 || u.is_connected == false);
}

TEST(User, set_name){
    u.set_name(name);
    EXPECT_EQ(name, u.name);
}

TEST(User, send_file){
    int s = u.files_to_send.size();
    u.send_file();
    EXPECT_TRUE(s != 0 || u.files_to_send.size() == 0);
    EXPECT_TRUE(s == 0 || u.files_to_send.size() == s - 1);
    u.files_to_send.push("file.txt");
    u.files_to_send.push("witam.txt");
    u.send_file();
    EXPECT_EQ(1, u.files_to_send.size());
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
