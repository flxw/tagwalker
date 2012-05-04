#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <stdio.h>
#include <sys/time.h>

#include <iostream>

#include <readline/readline.h>

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

    static std::string editArtistTagOnPrompt(const char *tag);
    static std::string editReleaseTagOnPrompt(const char *tag);
    static std::string editTitleTagOnPrompt(const char *tag);

public:
    static const char* rl_tag;
    static const char* rl_prompt;

private:
    static std::string editTagOnPrompt();

private:
    static const char* version;
    static const char* name;
};

#endif // USERINTERFACE_H
