#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <sys/time.h>

#include <iostream>

class UserInterface
{
public:
    UserInterface();

    static int printHelpMessage();
    static int printErrorMessage(int e);

    static void printFinalOutput(const unsigned int &movedFiles,
                                 const unsigned int &noHandle,
                                 const unsigned int &newDirs,
                                 const timeval &runtime,
                                 const unsigned int &delDirs);
    static void printFinalOutput(const unsigned int &movedFiles,
                                 const unsigned int &noHandle,
                                 const timeval &runtime,
                                 const unsigned int &delDirs);

private:
    static const char* version;
    static const char* name;
};

#endif // USERINTERFACE_H
