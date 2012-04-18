/**
 */
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#include <iostream>

#include "configuration.h"
#include "tagwalker.h"

using namespace std;

// program name and version
static const char* PROG_NAME = "twalk";
static const char* PROG_VERS = "0.0.1a";

// FIXME Ugly hack to be able to use member function with nftw
static TagWalker* tw_ptr = NULL;
static int tagwalker_direntryhandle_wrapper(const char *fpath,
                                            const struct stat *sb,
                                            int tflag,
                                            struct FTW *ftwbuf) {
    return tw_ptr->handleDirEntry(fpath, sb, tflag, ftwbuf);
}

static void printHelp(void)
{
    cout << PROG_NAME<< " version " << PROG_VERS;
    cout << endl;
    cout << "Usage: " << PROG_NAME
         << " <mode> <pattern> [OPTIONS] <working_directory>";
    cout << endl;
    cout << endl;
    cout << "Description:" << endl;
    cout << "Sort or rename your music files by a specific pattern that you have set" << endl;
    cout << endl;
    cout << "Options (standalone):" << endl;
    cout << "  -s\t Print a summary at the end" << endl;
    cout << "  -h\t Displays this message" << endl;
    cout << "  -t\t Test mode: Only show what would be done" << endl;
    cout << endl;
    cout << "Options (requiring arguments):" << endl;
    cout << "  -p <pattern>\t specify a pattern that shall be used to order/rename" << endl;
//    cout << endl;
//    cout << "Example for sorting every musicfile into a ARTIST/ALBUM structure:" << end;
//    cout << "   twalk reorder -p %a/%r" << endl; <<-- belongs into the manpage!

    /* Until now, the following sequences can be passed:
      %a - artist
      %r - release (==album)
      %t - track-number */
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        cerr << PROG_NAME;
        cerr << ": Please pass the parameters that tagwalker requires!" << endl;
        return 1;
    }

    Configuration arguments;

    // check for the operating mode
    // argv[1] is supposed to contain it
    if (strcmp(argv[1], "reorder") == 0) {
        arguments.setOpMode(Configuration::M_REORDER);
    } else {
        // avoid false error messages if the user really wanted to see the help
        if (strcmp(argv[1], "-h") != 0) {
            cerr << PROG_NAME;
            cerr << ": Please pass a valid mode as the first argument!" << endl;
            return 2;
        } else {
            printHelp();
            return 0;
        }
    }

    // process the rest
    for (int i=2; i<argc; ++i) {
        //process options that require arguments first
        if (strcmp(argv[i], "-p") == 0) {
            /* %a - artist | %r - album/release | %t - Track-number */
            //the argument is passed as: [...] -p "%a/%r" [...]
            //for now lets just assume the pattern is valid <- TODO
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
                    printHelp();
                    return 0;
                } else if (argv[i][j] == 'v') {
                    arguments.setVerbose(true);
                    arguments.setTestMode(false);
                } else if (argv[i][j] == 's') {
                    arguments.setSummaryFlag(true);
                } else if (argv[i][j] == 't') {
                    arguments.setTestMode(true);
                    arguments.setVerbose(false);
                }
            }
        } else {
            arguments.setWalkRoot(argv[i]);
        }
    }

    // verify that the starting point is valid
    if (!arguments.isWalkRootValid()) {
        cerr << PROG_NAME;
        cerr << ": The directory passed was not valid!" << endl;
        return 3;
    }

    // check for false patterns
    if (!arguments.isPatternValid()) {
        cerr << PROG_NAME;
        cerr << ": The pattern passed was not valid!" << endl;
        return 4;
    }
    return 0;

    tw_ptr = new TagWalker(arguments);
    /* Start walking the directory with the following options:
    Start the directory walk at: rootDir
    call function walkTheFile() for every item
    use a maximum of 20 fd's

    use the following flags:
      FTW_PHYS  -> makes nftw() ignore symbolic links
      FTW_DEPTH -> make nftw() traverse the file tree from back to front*/
    chdir(arguments.getWalkRoot().c_str());

    char pwd[4096];
    getcwd(pwd, 4096);
    arguments.setWalkRoot(pwd);
    timeval start, stop, result;

    gettimeofday(&start, NULL);
    nftw(pwd, tagwalker_direntryhandle_wrapper, 20, FTW_PHYS);
    gettimeofday(&stop, NULL);

    timersub(&stop, &start, &result);

    // print summary here
    if (arguments.hasSummaryFlag()) {
        cout << "Moved   " << tw_ptr->getMovedFileCount() << " files successfully" << endl;
        cout << "Created " << tw_ptr->getNewDirCount() << " new directories" << endl;
        cout << "Took    " << result.tv_sec + result.tv_usec/1000000.0 << "s" << endl;
    }

    delete tw_ptr;

    return 0;
}
