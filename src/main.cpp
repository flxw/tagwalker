#include <iostream>

#include "behavioursettings.h"

using namespace std;

static const char* PROGRAM_NAME_STR = "twalk";
static const char* VERSION_STR      = "0.0.1a";

static void printHelp(void) {
    cout << PROGRAM_NAME_STR << " version " << VERSION_STR << endl;
    cout << "Usage: " << PROGRAM_NAME_STR << " <mode> <pattern> [OPTIONS] <working_directory>" << endl;
    cout << endl;
    cout << "Description:" << endl;
    cout << "Sort/Rename your files by a specific pattern" << endl << endl;

    cout << "Options (requiring arguments):" << endl;
    cout << "  -p <pattern>\t specify a pattern that shall be used to order/rename" << endl;

    cout << "Options (standalone):" << endl;
    cout << "  -v\t Be talkative and print info useful for debugging" << end << endl;
    /* Until now, the following sequences can be passed:
      %a - artist
      %r - release (==album)
      %t - track-number */
}

int main(int argc, char** argv) {
    BehaviourSettings cfg;

    cfg.readCmdArgs(argc, argv);

    if (!cfg.isOk()) {
        printHelp();
        return 1;
    } else {
        cout << "Parsing the arguments was successful!\n";
        return 0;
    }
}
