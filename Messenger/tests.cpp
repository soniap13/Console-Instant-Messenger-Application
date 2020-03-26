#include "src/messenger_tools.cpp"
#include<gtest/gtest.h>

int port = 8084;
Conversation c(port);

TEST(Conversation, Contructor){
    EXPECT_EQ(port, c.conv_port);
    EXPECT_TRUE(c.user_listen_socket >= 1 || c.is_connected == false);
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
