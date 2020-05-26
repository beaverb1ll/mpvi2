#ifndef ARG_PARSE_HPP_
#define ARG_PARSE_HPP_

#include <map>
#include <string>
#include <vector>

class ArgParse {
 public:
  ArgParse();

  bool parse(const int argc, const char * const argv[]);

  bool register_flag(const std::string &name, const std::string &description);
  bool register_option(const std::string &name, const std::string &description, const std::string &default_val="");
  bool register_list(const std::string &name, const std::string &description);

  bool get_flag(const std::string &name);
  std::string get_option(const std::string &name);
  std::vector<std::string> get_list(const std::string &name);
  std::vector<std::string> get_non_options();

 private:
  enum Type {
    kTypeFlag = 0,
    kTypeOption,
    kTypeList,
    kNumTypes
  };
  struct Option {
    Type type;
    std::string name;
    char name_char = '\0';
    std::string desc;
    bool flag_value;
    std::string option_value;
    std::vector<std::string> list_value;

    std::string get_short_option() const {
      if(name_char == '\0') {
        return "";
      }
      if(type == kTypeFlag) {
        return std::string{name_char};
      }
      return std::string{name_char} + ":";
    }

    void print() const {
      std::string default_string;
      if(type == kTypeOption) {
        default_string = "="+option_value;
      }
      if(name_char) {
        printf("\t-%c[--%s]%s\t%s\n", name_char, name.c_str(), default_string.c_str(), desc.c_str());
      } else {
        printf("\t--%s%s\t%s\n", name.c_str(), default_string.c_str(), desc.c_str());
      }
    }
  };

  std::string get_long_name(const std::string &arg);
  char get_short_name(const std::string &arg);

  void update_option(Option &option);

  void help();
  void version();

  bool parsed_ = false;
  std::map<std::string, Option> options_;
  std::vector<std::string> non_options_;
};

#endif
