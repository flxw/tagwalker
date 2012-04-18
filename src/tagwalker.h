#ifndef TAGWALKER_H
#define TAGWALKER_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif //_XOPEN_SOURCE
#include <ftw.h>
#include <sys/wait.h>

#include <list>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "configuration.h"

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
    void handleMusicFile(const TagLib::FileRef& fr, std::string path);
    bool expandPattern(const TagLib::Tag *tr, std::string &expansion_str);

    // functions for handling files
    void RecursivelyMkdir(const std::string& path);
    void forkAndMove(const std::string& from, const std::string& to);

    // functions for handling pathnames
    std::string getBasename(const std::string &path);
    std::string getPathname(const std::string &path);

public:
    // getters for summary
    int getMovedFileCount() const;
    int getNewDirCount() const;

private:
    const Configuration &config;

    unsigned int movedFileCount;
    unsigned int newDirCount;
    unsigned int unableToHandleCount;

    std::list<std::string> testModeDirList;
};

#endif // TAGWALKER_H
