#ifndef TAGWALKER_H
#define TAGWALKER_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif //_XOPEN_SOURCE
#include <ftw.h>
#include <sys/wait.h>
#include <dirent.h>

#include <list>
#include <queue>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "configuration.h"

/* This class incorporates functions for walking the file tree
 * According to the information contained in a Configuration instance
 * patterns are expanded and files are renamed */

class TagWalker
{
public:
    TagWalker(const Configuration &conf);

    // functions that handle files
    int handleDirEntry(const char *fpath,
                   const struct stat *sb,
                   int tflag,
                   struct FTW *ftwbuf);

private:
    // functions for handling different opmodes
    void handleReorderMode(const TagLib::FileRef &fr, std::string path);
    void handleRenameMode(const TagLib::FileRef &fr, std::string path);

    // returns true if pattern expansion was successful
    bool expandPattern(const TagLib::Tag *tr, std::string &expansion_str);

    // functions for handling files
    void RecursivelyMkdir(const std::string &path);
    void forkAndMove(const std::string &from, const std::string& to);

    // functions for handling folders
    bool isDirectoryEmpty(const char* path);

    // functions for handling pathnames
    std::string getPathname(const std::string &path);
    std::string getBasename(const std::string &path);
    std::string getSuffix(const std::string &path);

public:
    // getters for summary
    unsigned int getMovedFileCount() const;
    unsigned int getNewDirCount() const;
    unsigned int getNoHandleCount() const;
    unsigned int getDelDirCount() const;

    // getters for testmode output
    std::queue<std::string> getTestModeOutputQueue();

private:
    const Configuration &config;

    unsigned int movedFileCount;
    unsigned int newDirCount;
    unsigned int unableToHandleCount;
    unsigned int removedDirCount;

    std::list<std::string> testModeDirList;
    std::queue<std::string> testModeOutputQueue;
};

#endif // TAGWALKER_H
