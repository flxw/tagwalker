#include "userinterface.h"

/**/
int readline_tag_insert_hook(void) {
    return rl_insert_text(UserInterface::rl_tag);
}
/**/

const char* UserInterface::version = "0.2";
const char* UserInterface::name    = "tagwalker";
const char* UserInterface::rl_tag = "";
const char* UserInterface::rl_prompt = "";

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
    const char* msg;

    switch (e) {
    case 1: msg = ": Please pass the parameters that tagwalker requires!"; break;
    case 2: msg = ": Please pass a valid mode as the first argument!"; break;
    case 3: msg = ": The given directory is not existent or usable!"; break;
    case 4: msg=  ": Please check the pattern for mistakes!"; break;
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
    std::cout << "Unhandled file count: " << noHandle << std::endl;

    if (delDirs > 0)
        std::cout << "Deleted directories " << delDirs    << std::endl;

    std::cout << "Took: " << runtime.tv_sec + runtime.tv_usec / 1000000.0 << "s" << std::endl;
}

std::string UserInterface::editArtistTagOnPrompt(const char *tag) {
    UserInterface::rl_prompt = "Edit matched artist tag: ";
    UserInterface::rl_tag = tag;

    return UserInterface::editTagOnPrompt();
}

std::string UserInterface::editReleaseTagOnPrompt(const char *tag) {
    UserInterface::rl_prompt = "Edit matched album tag: ";
    UserInterface::rl_tag = tag;


    return UserInterface::editTagOnPrompt();
}

std::string UserInterface::editTitleTagOnPrompt(const char *tag) {
    UserInterface::rl_prompt = "Edit matched title tag: ";
    UserInterface::rl_tag = tag;

    return UserInterface::editTagOnPrompt();
}

std::string UserInterface::editTagOnPrompt() {
    if (rl_startup_hook == NULL)
        rl_startup_hook = (Function*) readline_tag_insert_hook;

    char* str = readline(UserInterface::rl_prompt);
    std::string ret(str);
    delete str;

    // reset both static vars so that nftw won't choke
    UserInterface::rl_tag    = NULL;
    UserInterface::rl_prompt = NULL;

    return ret;
}
