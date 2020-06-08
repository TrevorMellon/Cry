
#include <application.h>

#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace cry;

Application::Application() 
{

}

Application::~Application()
{
    
}

void Application::parse_options(int argc, char **argv)
{
    po::options_description d("Allowed options");
    d.add_options()
            ("encrypt,e", "encrypt a file")
            ("decrypt,d", "decrypt a file")
            ("input,f", "input file")
            ;
    po::positional_options_description p;
    p.add("input", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(d).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("encrypt")
            && vm.count("input"))
    {
        std::cout << "Encrypt \"" << vm["input"].as< std::string >() << "\"" << std::endl;
        //e
    }
    else if(vm.count("decrypt")
            && vm.count("input"))
    {
        std::cout << "Decrypt \"" << vm["input"].as< std::string>() << "\"" << std::endl;
        //d
    }



}
