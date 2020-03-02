#include "arg_parse.hpp"

#include <cstring>
#include <functional>
#include <vector>

#include <unistd.h>
#include <getopt.h>

#include "string_utils.hpp"

ArgParse::ArgParse() {
  register_flag("version,v", "Print version information");
  register_flag("help,h", "Show this menu");
}

bool ArgParse::parse(const int argc, char * const  argv[]) {
  if(parsed_) {
    return false;
  }

  std::vector<struct option> long_options;
  long_options.reserve(options_.size());
  std::map<char, std::reference_wrapper<ArgParse::Option>> char_map;

  std::string short_options;
  for(auto &opt : options_) {
    struct option new_opt;
    new_opt.name = opt.second.name.c_str();
    new_opt.has_arg = (opt.second.type == ArgParse::kTypeFlag) ? no_argument : required_argument;
    new_opt.flag = nullptr;
    new_opt.val = 0;
    long_options.push_back(new_opt);
    short_options += opt.second.get_short_option();
    if(opt.second.name_char) {
      char_map.emplace(std::make_pair(opt.second.name_char, std::reference_wrapper<ArgParse::Option>(opt.second)));
    }
  }
  short_options = ":" + short_options;

  // add terminator
  struct option empty_opt;
  memset(&empty_opt, 0, sizeof(empty_opt));
  long_options.push_back(empty_opt);

  opterr=0; // don't show error messages to console
  while(true) {

    int option_index = 0;
    const int c = getopt_long(argc, argv, short_options.c_str(), long_options.data(), &option_index);

    if (c == -1) {
      break;
    }
    switch(c) {
      case 0: {
        // got a long option
        Option &option = options_[long_options[option_index].name];
        update_option(option);
      } break;

      case 'h':
        help();
        break;

      case 'v':
        version();
        break;

      case ':':
        throw std::runtime_error("Argument \'" + std::string{argv[optind-1]} + "\' requires value");
        break;

      case '?':
        throw std::runtime_error("Non-registered option encountered: " + std::string{argv[optind-1]});
        break;

      default: {
        auto search = char_map.find(c);
        if(search != char_map.end()) {
          auto &option = search->second.get();
          update_option(option);
        }
        }
        break;
    }
  }
  for (int i = optind; i < argc; i++) {
    non_options_.emplace_back(argv[i]);
  }

  parsed_ = true;
  return true;
}

void ArgParse::update_option(Option &option) {
  if(option.name == "help") {
    help();
  } else if(option.name == "version") {
    version();
  }
  if(option.type == ArgParse::kTypeFlag) {
    option.flag_value = true;
  } else if(option.type == ArgParse::kTypeOption) {
    option.option_value = optarg;
  } else if(option.type == ArgParse::kTypeList) {
    option.list_value.push_back(optarg);
  }
}

std::string ArgParse::get_long_name(const std::string &arg) {
  return arg.substr(0, arg.find_first_of(","));
}
char ArgParse::get_short_name(const std::string &arg) {
  const auto index = arg.find_first_of(",");
  if(index == std::string::npos) {
    return '\0';
  }
  const std::string short_name = arg.substr(index+1);
  if(short_name.length() > 1) {
    throw std::runtime_error("short argument must not be greater then 1");
  }
  return short_name[0];
}

void ArgParse::help() {
  printf("Usage:\n");
  for(const auto &opt : options_) {
    opt.second.print();
  }
  exit(0);
}

void ArgParse::version() {
  printf("VERSION CALLED\n");
  exit(0);
}

bool ArgParse::register_flag(const std::string &name, const std::string &description) {
  if(parsed_) {
    return false;
  }

  const auto long_arg = get_long_name(name);
  const char short_arg = get_short_name(name);

  if(long_arg == "") {
    return false;
  }

  Option &option = options_[long_arg];
  if(option.name != "") {
    return false;
  }
  option.type = ArgParse::kTypeFlag;
  option.name = long_arg;
  option.name_char = short_arg;
  option.flag_value = false;
  option.desc = description;
  return true;
}

bool ArgParse::register_option(const std::string &name, const std::string &description, const std::string &default_val) {
  if(parsed_) {
    return false;
  }

  const auto long_arg = get_long_name(name);
  const char short_arg = get_short_name(name);

  if(long_arg == "") {
    return false;
  }

  Option &option = options_[long_arg];
  if(option.name != "") {
    return false;
  }
  option.type = ArgParse::kTypeOption;
  option.name = long_arg;
  option.name_char = short_arg;
  option.option_value = default_val;
  option.desc = description;
  return true;
}

bool ArgParse::register_list(const std::string &name, const std::string &description) {
  if(parsed_) {
    return false;
  }

  const auto long_arg = get_long_name(name);
  const char short_arg = get_short_name(name);
  if(long_arg == "") {
    return false;
  }

  Option &option = options_[long_arg];
  if(option.name != "") {
    return false;
  }
  option.type = ArgParse::kTypeList;
  option.name = long_arg;
  option.name_char = short_arg;
  option.desc = description;
  return true;
}

bool ArgParse::get_flag(const std::string &name) {
  const auto long_arg = get_long_name(name);
  const Option &option = options_[long_arg];
  if(option.name == "") {
    throw std::runtime_error("Unregistered flag requested");
  }
  if(option.type != ArgParse::kTypeFlag) {
    throw std::runtime_error("Requested argument not registed as flag");
  }
  return option.flag_value;
}

std::string ArgParse::get_option(const std::string &name) {
  const auto long_arg = get_long_name(name);
  const Option &option = options_[long_arg];
  if(option.name == "") {
    throw std::runtime_error("Unregistered option requested");
  }
  if(option.type != ArgParse::kTypeOption) {
    throw std::runtime_error("Requested argument not registed as option");
  }
  return option.option_value;
}

std::vector<std::string> ArgParse::get_list(const std::string &name) {
  const auto long_arg = get_long_name(name);
  const Option &option = options_[long_arg];
  if(option.name == "") {
    throw std::runtime_error("Unregistered list requested");
  }
  if(option.type != ArgParse::kTypeList) {
    throw std::runtime_error("Requested argument not registed as list");
  }
  return option.list_value;
}

std::vector<std::string> ArgParse::get_non_options() {
  return non_options_;
}
