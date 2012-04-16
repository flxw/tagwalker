#ifndef BEHAVIOURSETTINGS_H
#define BEHAVIOURSETTINGS_H

#include <string>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

class BehaviourSettings
{
public:
    BehaviourSettings();

    // methods to read in arguments and verify them
    void readCmdArgs(const int &argc, char **argv);
    bool verifyWalkRoot();
    inline bool isOk() const {return parsingWasSuccessful;}

private:
    enum OPMODE {M_REORDER};

    bool parsingWasSuccessful;

    // properties concerning program behaviour
    std::string walkRoot;
    std::string pattern;

    OPMODE mode;

    bool verboseFlag;
};

#endif // BEHAVIOURSETTINGS_H
