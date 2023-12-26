
#include <application.h>

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <cry/config.h>

namespace po = boost::program_options;

using namespace cry;

// ==========================================================================

void Application::ParseOptions() {
  po::options_description d("Allowed options");
  d.add_options()
#if CRY_ENCRYPT
      ("encrypt,e", "encrypt a file")
#endif
#if CRY_DECRYPT
          ("decrypt,d", "decrypt a file")
#endif
#if CRY_DECRYPT || CRY_ENCRYPT
              ("password,p", po::value<std::string>(), "set the given password")
#endif
                  ("input,f", po::value<std::string>(), "input file")(
                      "output,o", po::value<std::string>(), "output file")(
                      "version", "Get the application's version")(
                      "help,h", "Display this help");
  po::positional_options_description p;
  p.add("input", -1);
  po::variables_map vm;
  po::store(
      po::command_line_parser(_argc, _argv).options(d).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("version")) {
    DisplayVersion();
    return;
  }

  if (vm.count("help")) {
    std::cout << d << std::endl;
    return;
  }

  if (vm.count("output")) {
    _cry->SetOutput(vm["output"].as<std::string>());
  }

#if CRY_DECRYPT || CRY_ENCRYPT

  if (vm.count("password")) {
#if CRY_STDOUT
    std::cout << "Using Password: " << vm["password"].as<std::string>()
              << std::endl;
#endif
    _cry->SetPassword(vm["password"].as<std::string>());
  } else {
#if CRY_STDOUT
    std::cout << "Using default password" << std::endl;
#endif
    _cry->SetPassword(cry_default_password);
  }

#endif

  if (vm.count("encrypt") && vm.count("input")) {
#if CRY_ENCRYPT
#if CRY_STDOUT
    std::cout << "Encrypt \"" << vm["input"].as<std::string>() << "\""
              << std::endl;
#endif // DBG
    _cry->Encrypt(vm["input"].as<std::string>());
#endif
  } else if (vm.count("decrypt") && vm.count("input")) {
#if CRY_DECRYPT
#if CRY_STDOUT
    std::cout << "Decrypt \"" << vm["input"].as<std::string>() << "\""
              << std::endl;
#endif // DBG
    _cry->Decrypt(vm["input"].as<std::string>());
#endif
  }
}

void Application::DisplayVersion() const {
  std::cout << "Cry - A Cryptor Application" << std::endl;
  std::cout << "Git Repo: " << git_repo << std::endl;
  std::cout << "Git Branch: " << git_branch << std::endl;
  std::cout << "Git Hash: " << git_hash << std::endl;
  std::cout << std::endl;
  std::cout << cry_version_string << std::endl;
}
