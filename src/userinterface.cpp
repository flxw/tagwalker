#include "userinterface.h"

const char* UserInterface::version = "0.2";
const char* UserInterface::name    = "tagwalker";

UserInterface::UserInterface()
{
}

int UserInterface::printHelpMessage() {
    std::cout << UserInterface::name << " version " << UserInterface::version;
    std::cout << std::endl;
    std::cout << "Usage: " << UserInterface::name << " <mode> <pattern> [OPTIONS] <working_directory>";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Description:" << std::endl;
    std::cout << "sort or rename your music files by a specific pattern" << std::endl;
    std::cout << "Also check for specific patterns in the tags" << std::endl;
    std::cout << "in case you want to cleanup before sorting" << std::endl;
    std::cout << std::endl;
    std::cout << "Options (standalone):" << std::endl;
    std::cout << "  -s\t Print a summary at the end" << std::endl;
    std::cout << "  -h\t Displays this message" << std::endl;
    std::cout << "  -t\t Test mode: Only show what would be done" << std::endl;
    std::cout << "  -c\t Cleanup: if any empty folders are found, they are deleted" << std::endl;
    std::cout << "    \t for info on using this with the testflag see the manpage" << std::endl;
    std::cout << std::endl;
    std::cout << "Options (requiring arguments):" << std::endl;
    std::cout << "  -p <pattern>\t specify a pattern that shall be used to order/rename" << std::endl;

    return 0;
}

int UserInterface::printErrorMessage(int e) {
    char* msg;

    switch (e) {
    case 1: msg = ": Please pass the parameters that tagwalker requires!";
    case 2: msg = ": Please pass a valid mode as the first argument!";
    case 3: msg = ": The given directory is not existent or usable!";
    case 4: msg=  ": Please check the pattern for mistakes!";
    }

    std::cerr << UserInterface::name << msg << std::endl;

    return e;
}

void UserInterface::printFinalOutput(const unsigned int &movedFiles,
                                     const unsigned int &noHandle,
                                     const unsigned int &newDirs,
                                     const timeval &runtime,
                                     const unsigned int &delDirs) {
    std::cout << "Moved file count:     " << movedFiles << std::endl;
    std::cout << "Unhandled file count: " << noHandle   << std::endl;
    std::cout << "Created directories : " << newDirs    << std::endl;

    if (delDirs > 0)
        std::cout << "Deleted directories : " << delDirs    << std::endl;

    std::cout << "Took: " << runtime.tv_sec + runtime.tv_usec / 1000000.0 << "s" << std::endl;
}

void UserInterface::printFinalOutput(const unsigned int &movedFiles,
                                     const unsigned int &noHandle,
                                     const timeval &runtime,
                                     const unsigned int &delDirs) {
    std::cout << "Renamed file count: " << movedFiles << std::endl;

    if (delDirs > 0)
        std::cout << "Deleted directories " << delDirs    << std::endl;

    std::cout << "Took: " << runtime.tv_sec + runtime.tv_usec / 1000000.0 << "s" << std::endl;
}
