/*
 * This findle handles all output of the program
 * and 'uses' the classes to provide functionality
 */
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#include "userinterface.h"
#include "walker.h"

using namespace std;

// FIXME Ugly hack to be able to use member function with nftw
static Walker* walker_ptr = NULL;
static int walker_direntryhandle_wrapper(const char *fpath,
                                            const struct stat *sb,
                                            int tflag,
                                            struct FTW *ftwbuf) {
    return walker_ptr->handleDirEntry(fpath, sb, tflag, ftwbuf);
}

int main(int argc, char** argv)
{
    // Not enough arguments were passed - quit
    if (argc == 1) {
        return(UserInterface::printErrorMessage(1));
    }

    Configuration arguments;

    // check for the operating mode
    // argv[1] is supposed to contain it
    if (strcmp(argv[1], "reorder") == 0) {
        arguments.setOpMode(Configuration::M_REORDER);
    } else if (strcmp(argv[1], "rename") == 0) {
        arguments.setOpMode(Configuration::M_RENAME);
    } else if (strcmp(argv[1], "check") == 0) {
        arguments.setOpMode(Configuration::M_CHECK);
    } else {
        // avoid false error messages if the user really wanted to see the help
        if (strcmp(argv[1], "-h") != 0) {
            return(UserInterface::printErrorMessage(2));
        } else {
            return(UserInterface::printHelpMessage());
        }
    }

    // process the remaining parameters; should only be opts
    for (int i=2; i<argc; ++i) {
        //process options that require arguments first
        if (strcmp(argv[i], "-p") == 0) {
            /* %a - artist | %r - album/release | %t - Track-number */
            //the argument is passed as: [...] -p "%a/%r" [...]
            arguments.setPattern(argv[++i]);
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
                    return(UserInterface::printHelpMessage());
                } else if (argv[i][j] == 'c') {
                    arguments.setCleanup(true);
                } else if (argv[i][j] == 's') {
                    arguments.setSummaryFlag(true);
                } else if (argv[i][j] == 't') {
                    arguments.setTestMode(true);
                }
            }
        } else {
            arguments.setWalkRoot(argv[i]);
        }
    }

    // verify that the starting point is valid
    if (!arguments.isWalkRootValid()) {
        return UserInterface::printErrorMessage(3);
    }
    // check for an invalid pattern
    if (!arguments.isPatternValid()) {
        return UserInterface::printErrorMessage(4);
    }

    walker_ptr = new Walker(arguments);

    // we want to be able to measure program runtime
    timeval start, stop, result;

    /* Start walking the directory with the following options:
    Start the directory walk at: rootDir
    call function walkTheFile() for every item
    use a maximum of 20 fd's

    use the following flags:
      FTW_PHYS  -> makes nftw() ignore symbolic links
      FTW_DEPTH -> make nftw() traverse the file tree from back to front*/
    gettimeofday(&start, NULL); // start epoch time
    nftw(arguments.getWalkRoot().c_str(), walker_direntryhandle_wrapper, 20, FTW_DEPTH | FTW_PHYS);
    gettimeofday(&stop, NULL); // stop epoch time

    timersub(&stop, &start, &result);

    if (arguments.hasTestMode()) {
        queue<string> q = walker_ptr->getTestOutputQueue();

        while (!q.empty()) {
            cout << q.front() << endl;
            q.pop();
        }
    }

    if (arguments.hasSummaryFlag()) {
        if (arguments.getOpMode() == Configuration::M_REORDER) {
            UserInterface::printFinalOutput(walker_ptr->getMovedFileCount(),
                                            walker_ptr->getUnableToHandleCount(),
                                            walker_ptr->getNewDirCount(),
                                            result,
                                            walker_ptr->getDelDirCount());
        } else if (arguments.getOpMode() == Configuration::M_RENAME) {
            UserInterface::printFinalOutput(walker_ptr->getMovedFileCount(),
                                            walker_ptr->getUnableToHandleCount(),
                                            result,
                                            walker_ptr->getDelDirCount());
        }
    }

    delete walker_ptr;

    return 0;
}
