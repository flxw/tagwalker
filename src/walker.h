#ifndef WALKER_H
#define WALKER_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif //_XOPEN_SOURCE
#include <ftw.h>

#include <queue>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "configuration.h"
#include "direntryhandle.h"
#include "userinterface.h"

class Walker
{
public:
    Walker(const Configuration &cfg);

    int handleDirEntry(const char *fpath,
                   const struct stat *sb,
                   int tflag,
                   struct FTW *ftwbuf);

    // getter relays
    unsigned int getMovedFileCount() const;
    unsigned int getNewDirCount() const;
    unsigned int getDelDirCount() const;
    unsigned int getUnableToHandleCount() const;
    std::queue<std::string> getTestOutputQueue();

private:
    bool actOnCheckResult(int cr, const TagLib::FileRef &fr);
    bool expandPattern(const TagLib::Tag *tr, std::string &expansion_str);

private:
    const Configuration &config;
    DirEntryHandle deHandle;

    unsigned int unableToHandleCount;
};

#endif // WALKER_H
