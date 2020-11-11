
#include <application.h>

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace cry;

// ==========================================================================

Application::Application()
{
    _cry = new Cry();
}

// ==========================================================================

Application::~Application()
{
    delete _cry;
}

// ==========================================================================

void Application::parse_options ( int argc, char **argv )
{
    po::options_description d ( "Allowed options" );
    d.add_options()
#if CRY_ENCRYPT
    ( "encrypt,e", "encrypt a file" )
#endif
#if CRY_DECRYPT
    ( "decrypt,d", "decrypt a file" )
#endif
#if CRY_DECRYPT || CRY_ENCRYPT
    ( "password,p", po::value<std::string>(), "set the given password" )
#endif
    ( "input,f", po::value<std::string>(), "input file" )
    ( "output,o", po::value<std::string>(), "output file" )
    ;
    po::positional_options_description p;
    p.add ( "input", -1 );
    po::variables_map vm;
    po::store ( po::command_line_parser ( argc, argv ).options ( d ).positional ( p ).run(), vm );
    po::notify ( vm );

    if ( vm.count ( "output" ) )
    {
        _cry->setOutput (
                vm["output"].as<std::string>()
        );
    }

#if CRY_DECRYPT || CRY_ENCRYPT

    if ( vm.count ( "password" ) )
    {
#if DBG
        std::cout << "Using Password: " << vm["password"].as<std::string>() << std::endl;
#endif
        _cry->setPassword (
                vm["password"].as<std::string>()
        );
    }

#endif

    if ( vm.count ( "encrypt" )
            && vm.count ( "input" ) )
    {
#if CRY_ENCRYPT
# if DBG
        std::cout << "Encrypt \"" << vm["input"].as< std::string >() << "\"" << std::endl;
# endif//DBG        
        _cry->encrypt ( vm["input"].as< std::string >() );
#endif
    }
    else if ( vm.count ( "decrypt" )
              && vm.count ( "input" ) )
    {
#if CRY_DECRYPT
# if DBG
        std::cout << "Decrypt \"" << vm["input"].as< std::string>() << "\"" << std::endl;
# endif  //DBG        
        _cry->decrypt ( vm["input"].as<std::string>() );
#endif
    }

}


