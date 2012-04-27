#ifndef DirEntryHandle_H
#define DirEntryHandle_H



#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <list>
#include <queue>

#include "configuration.h"

/* This class incorporates functions for walking the file tree
 * According to the information contained in a Configuration instance
 * patterns are expanded and files are renamed */

class DirEntryHandle
{
public:
    DirEntryHandle(const Configuration &conf);

    void handleDirectory(const char *fpath);

    // getters for summary
    unsigned int getMovedFileCount() const;
    unsigned int getNewDirCount() const;
    unsigned int getDelDirCount() const;

    // getters for testmode output
    std::queue<std::string> getTestModeOutputQueue();

    // functions for handling different opmodes
    void handleReorderMode(std::string &expandedPattern, const std::string path);
    void handleRenameMode(std::string &expandedPattern, const std::string path);

    // Enums
    enum CheckResult {CR_OKAY, CR_ARTIST, CR_RELEASE, CR_TITLE};

private:
    // functions for handling files
    void RecursivelyMkdir(const std::string &path);
    void forkAndMove(const std::string &from, const std::string& to);

    // functions for handling folders
    bool isDirectoryEmpty(const char* path);

    // functions for handling pathnames
    std::string getPathname(const std::string &path);
    std::string getBasename(const std::string &path);
    std::string getSuffix(const std::string &path);


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
