#include <iostream>
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"
#include "webdriver_route_patterns.h"
#include "extension_wpe/uinput_manager.h"
#include "extension_wpe/uinput_event_dispatcher.h"
#include "extension_wpe/wpe_view_creator.h"
#include "extension_wpe/wpe_view_creator.h"
#include "extension_wpe/wpe_view_enumerator.h"
#include "extension_wpe/wpe_view_executor.h"
#include "extension_wpe/wpe_key_converter.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

#include "webdriver_switches.h"
#include <glib.h>

void PrintVersion();
void PrintHelp();
void InitUInputClient();

int main(int argc, char *argv[])
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    CommandLine cmd_line(CommandLine::NO_PROGRAM);
    cmd_line.InitFromArgv(argc, argv);

    // check if --help CL argument is present
    if (cmd_line.HasSwitch("help")) {
        PrintHelp();
        return 0;
    }

    // check if --version CL argument is present
    if (cmd_line.HasSwitch("version")) {
      PrintVersion();
      return 0;
    }

    // Create WPE Driver instance
    // WPE Webkit View
    webdriver::ViewCreator* wpeViewCreator = new webdriver::WpeViewCreator();
    webdriver::ViewFactory::GetInstance()->AddViewCreator(wpeViewCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WpeViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::WpeViewCmdExecutorCreator());

    /* Parse command line */
    webdriver::Server* wd_server = webdriver::Server::GetInstance();
    if (0 != wd_server->Configure(cmd_line)) {
        std::cout << "Error while configuring WD server, exiting..." << std::endl;
        return 1;
    }
    std::cout << "Got webdriver server instance..." << std::endl;

    InitUInputClient();

    /* Start webdriver */
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    int startError = wd_server->Start();
    if (startError){
        std::cout << "Error while starting server, errorCode " << startError << std::endl;
        return startError;
    }
    std::cout << "webdriver server started..." << std::endl;

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    return 0; 
}

void InitUInputClient() {
    // start user input device
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    UInputManager *manager = UInputManager::getInstance();
    if (!manager->isReady())
    {
         manager->registerUinputDevice();

    }
    UInputEventDispatcher::getInstance()->registerUInputManager(manager);
}

void PrintVersion() {
    std::cout <<webdriver::VersionInfo::CreateVersionString()<< std::endl;
}

void PrintHelp() {
    std::cout << "Usage: WebDriver [--OPTION=VALUE]..."                                             << std::endl
              << "Starts WPEWebDriver server"                                                       << std::endl
              << ""                                                                                 << std::endl
              << "OPTION         DEFAULT VALUE      DESCRIPTION"                                    << std::endl
              << "http-threads   4                  The number of threads to use for handling"      << std::endl
              << "                                  HTTP requests"                                  << std::endl
              << "log-path       ./webdriver.log    The path to use for the WPEWebDriver server"    << std::endl
              << "                                  log"                                            << std::endl
              << "root           ./web              The path of location to serve files from"       << std::endl
              << "port           9517               The port that WPEWebDriver listens on"          << std::endl
              << "silence        false              If true, WPEWebDriver will not log anything"    << std::endl
              << "                                  to stdout/stderr"                               << std::endl
              << "verbose        false              If true, WPEWebDriver will log lots of stuff"   << std::endl
              << "                                  to stdout/stderr"                               << std::endl
              << "url-base                          The URL path prefix to use for all incoming"    << std::endl
              << "                                  WebDriver REST requests. A prefix and postfix"  << std::endl
              << "                                  '/' will automatically be appended if not"      << std::endl
              << "                                  present"                                        << std::endl
              << "config                            The path to config file (e.g. config.json) in"  << std::endl
              << "                                  JSON format with specified WD parameters as"    << std::endl
              << "                                  described above (port, root, etc.)"             << std::endl
              << "wi-server      false              If true, web inspector will be enabled"         << std::endl
              << "wi-port        9222               Web inspector listening port"                   << std::endl
              << "version                           Print version information to stdout and exit"   << std::endl
              << "                                  format: login:password@ip:port"                 << std::endl
              << "uinput         false              If option set, user input device"               << std::endl
              << "                                  will be registered in the system"               << std::endl
              << "whitelist                         The path to whitelist file (e.g. whitelist.xml)"<< std::endl
              << "                                  in XML format with specified list of IP with"   << std::endl
              << "                                  allowed/denied commands for each of them."      << std::endl
              << "webserver-cfg                     The path to mongoose config file"               << std::endl
              << "                                  (e.g. /path/to/config.json) in JSON format with"<< std::endl
              << "                                  specified mongoose start option"                << std::endl
              << "                                  (extra-mime-types, listening_ports, etc.)"      << std::endl
              << "                                  Option from webserver config file will have"    << std::endl
              << "                                  more priority than commandline param"           << std::endl
              << "                                  that specify the same option."                  << std::endl;
}
