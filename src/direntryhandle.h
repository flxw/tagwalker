#ifndef DirEntryHandle_H
#define DirEntryHandle_H

#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <list>
#include <queue>

#include "configuration.h"

/* This class handles moving and renaming files
 * according to the information contained in a Configuration instance */

class DirEntryHandle
{
public:
    DirEntryHandle(const Configuration &conf);

    // Enums
    enum CheckResult {CR_OKAY, CR_ARTIST, CR_RELEASE, CR_TITLE};

    // getters for summary
    unsigned int getMovedFileCount() const;
    unsigned int getNewDirCount() const;
    unsigned int getDelDirCount() const;

    // getters for testmode output
    std::queue<std::string> getTestModeOutputQueue();

    // functions for handling different opmodes
    void handleDirectory(const char *fpath);
    void handleReorderMode(std::string &expandedPattern, const std::string path);
    void handleRenameMode(std::string &expandedPattern, const std::string path);
    int handleCheckMode(const char *artist,
                        const char *release,
                        const char *title);

private:
    // functions for handling files
    void RecursivelyMkdir(const std::string &path);
    void forkAndMove(const std::string &from, const std::string& to);

    // functions for handling folders
    bool isDirectoryEmpty(const char* path);

    // functions for handling pathnames
    static std::string getPathname(const std::string &path);
    static std::string getBasename(const std::string &path);
    static std::string getSuffix(const std::string &path);


private:
    const Configuration &config;

    unsigned int movedFileCount;
    unsigned int newDirCount;
    unsigned int removedDirCount;
    unsigned int forkFailCount;

    std::list<std::string> testModeDirList;
    std::queue<std::string> testModeOutputQueue;
};

#endif // DirEntryHandle_H
