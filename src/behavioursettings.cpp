#include "behavioursettings.h"

BehaviourSettings::BehaviourSettings()
{
    this->parsingWasSuccessful = false;
    this->verboseFlag          = false;
}

void BehaviourSettings::readCmdArgs(const int &argc, char **argv) {
    // if anything is going wrong, the parsingWasSuccessful-Flag will be set to false
    this->parsingWasSuccessful = true;

    if (argc == 1) {
        this->parsingWasSuccessful = false;
        return;
    }

    // argv[1] contains the operating mode:
    if (strcmp(argv[1], "reorder") == 0) {
        this->mode = M_REORDER;
    } else {
        this->parsingWasSuccessful = false;
        return;
    }

    // process commandline arguments
    for (int i=2; i<argc; ++i)
    {
        //process options that require arguments first
        if (strcmp(argv[i], "-p") == 0) {
            /* %a - artist | %r - album/release | %t - Track-number */
            // the argument is passed as: [...] -p "%a/%r" [...]
            // TODO for now lets just assume the pattern is valid
            this->pattern = std::string(argv[++i]);
        }
        //avoid the preceding "-"
        else if (argv[i][0] == '-') {
            /* Begin walking the argument string from the back to the front
                   Direction does not matter, because the options do not depend
                   on one another.
               Example:
               -wasd
                   ^ i points here
                  ^  i points here after the first pass
               ...*/
            for (int j=strlen(argv[i])-1; j>0; --j) {
                if (argv[i][j] == 'h') {
                    this->parsingWasSuccessful = false;
                    return;
                } else if (argv[i][j] == 'v') {
                    this->verboseFlag = true;
                }
            }
        } else {
            this->walkRoot = std::string(argv[i]);
        }
    }

    // Verify that we have a starting point and that it is valid
    if (this->walkRoot.empty() || !this->verifyWalkRoot())
    {
        this->parsingWasSuccessful = false;
    }
}

bool BehaviourSettings::verifyWalkRoot() {
    bool perms = access(this->walkRoot.c_str(), R_OK | W_OK | X_OK | F_OK) == 0;
    bool isDir = opendir(this->walkRoot.c_str()) != NULL;

    return perms && isDir;
}
