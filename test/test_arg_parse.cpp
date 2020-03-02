#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "arg_parse.hpp"

using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::_;

class ArgParseTest : public ::testing::Test {
 protected:
  void SetUp() override {
    optind = 1;
  }

  void TearDown() override {
  }

};

TEST_F(ArgParseTest, empty_flag) {
  ArgParse arg;
  EXPECT_FALSE(arg.register_flag("", "a test"));
}

TEST_F(ArgParseTest, reg_flag) {
  ArgParse arg;
  EXPECT_TRUE(arg.register_flag("TestFlag", "a test"));

  EXPECT_FALSE(arg.register_flag("TestFlag", "a test"));
}

TEST_F(ArgParseTest, empty_option) {
  ArgParse arg;
  EXPECT_FALSE(arg.register_option("", "a test"));
}

TEST_F(ArgParseTest, reg_option) {
  ArgParse arg;
  EXPECT_TRUE(arg.register_option("TestOption", "a test"));

  EXPECT_FALSE(arg.register_option("TestOption", "a test"));
}


TEST_F(ArgParseTest, empty_list) {
  ArgParse arg;
  EXPECT_FALSE(arg.register_list("", "a test"));
}

TEST_F(ArgParseTest, reg_list) {
  ArgParse arg;
  EXPECT_TRUE(arg.register_list("TestList", "a test"));

  EXPECT_FALSE(arg.register_list("TestList", "a test"));
}

TEST_F(ArgParseTest, parse_flag) {
  ArgParse arg;
  arg.register_flag("TestFlag", "flag desc");
  char *argv[2]{"app_name", "--TestFlag"};
  ASSERT_TRUE(arg.parse(2, argv));

  EXPECT_TRUE(arg.get_flag("TestFlag"));

  ASSERT_THROW(arg.get_flag("testflag"), std::runtime_error);
}

TEST_F(ArgParseTest, parse_option) {
  ArgParse arg;
  arg.register_option("TestOption", "option desc", "cats");
  char *argv[2]{"app_name", "--TestOption=Hello"};
  ASSERT_TRUE(arg.parse(2, argv));

  EXPECT_STREQ(arg.get_option("TestOption").c_str(), "Hello");

  ASSERT_THROW(arg.get_option("testoption"), std::runtime_error);
}

TEST_F(ArgParseTest, parse_list) {
  ArgParse arg;
  arg.register_list("test-list", "list desc");
  char * argv[4]{"app_name", "--test-list=a", "--test-list=b", "--test-list=c" };
  ASSERT_TRUE(arg.parse(4, argv));

  const std::vector<std::string> truth{"a", "b", "c"};

  EXPECT_EQ(arg.get_list("test-list"), truth);

  ASSERT_THROW(arg.get_list("TestList"), std::runtime_error);
}

TEST_F(ArgParseTest, non_options) {
  ArgParse arg;
  arg.register_option("test-option", "option desc");


  char * argv[5]{"app_name", "aa", "--test-option=b", "dd", "cc" };
  ASSERT_TRUE(arg.parse(5, argv));

  const std::vector<std::string> truth{"aa", "dd", "cc"};

  EXPECT_EQ(arg.get_non_options(), truth);
  EXPECT_EQ(arg.get_option("test-option"), "b");
}
