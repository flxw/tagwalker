#include "direntryhandle.h"

DirEntryHandle::DirEntryHandle(const Configuration &conf) : config(conf)
{
    this->movedFileCount      = 0;
    this->newDirCount         = 0;
    this->removedDirCount     = 0;
    this->forkFailCount       = 0;
}

void DirEntryHandle::handleDirectory(const char *fpath) {

    if (this->config.shouldCleanup() && this->isDirectoryEmpty(fpath)) {
        if (this->config.hasTestMode()) {
            this->testModeOutputQueue.push(std::string("Delete `") + std::string(fpath) + std::string("'"));
        } else {
            rmdir(fpath);
        }
        ++this->removedDirCount;
    }
}

void DirEntryHandle::handleReorderMode(std::string &expandedPattern, const std::string path) {
    expandedPattern = this->config.getWalkRoot() + expandedPattern;
    expandedPattern.append(this->getBasename(path));

    // only act if the two directories differ
    if ( path != expandedPattern ) {
        this->RecursivelyMkdir(expandedPattern);
        this->forkAndMove(path, expandedPattern);
    }
}

void DirEntryHandle::handleRenameMode(std::string &expandedPattern, const std::string path) {
    expandedPattern = this->getPathname(path) + std::string("/") + expandedPattern;
    expandedPattern.append(this->getSuffix(path));

    // only act if the two directories differ
    if ( path != expandedPattern ) {
        this->forkAndMove(path, expandedPattern);
    }
}

int DirEntryHandle::handleCheckMode(const char* artist,
                                                            const char* release,
                                                            const char* title) {
    int checkResult = 0;

    /// handle artist regex here
    if (this->config.hasArtistRegexBuffer()) {
        if (regexec(this->config.getArtistRegexBufferPtr(), artist, 0, 0, 0) == 0) {
            checkResult |= DirEntryHandle::CR_ARTIST;
        }
    }

    /// handle release regex here
    if (this->config.hasReleaseRegexBuffer()) {
        if (regexec(this->config.getReleaseRegexBufferPtr(), release, 0, 0, 0) == 0) {
            checkResult |= DirEntryHandle::CR_RELEASE;
        }
    }

    /// handle title regex here
    if (this->config.hasArtistRegexBuffer()) {
        if (regexec(this->config.getArtistRegexBufferPtr(), title, 0, 0, 0) == 0) {
            checkResult |= DirEntryHandle::CR_TITLE;
        }
    }

    return (checkResult == 0) ? CR_OKAY : checkResult;
}

void DirEntryHandle::RecursivelyMkdir(const std::string &path) {
    std::string p = this->getPathname(path);
    unsigned int i = p.find('/', this->config.getWalkRoot().length()+1);

    // Test for directory existence first
    if ( access(p.c_str(), R_OK|W_OK|X_OK|F_OK) == 0) {
        return;
    }

    while (i != 0) {
        i = p.find('/', i);
        const std::string buf = p.substr(0, i++);

        if (access(buf.c_str(), R_OK | W_OK | X_OK | F_OK) != 0) {

            if (this->config.hasTestMode()) {
                bool alreadyCreated = false;

                for(std::list<std::string>::const_iterator it = this->testModeDirList.begin();
                    it != this->testModeDirList.end(); ++it) {
                    if (*it == buf) {
                        alreadyCreated = true;
                        break;
                    }
                }

                if (!alreadyCreated) {
                    this->testModeOutputQueue.push(std::string("Create directory `") + buf + std::string("'"));
                    this->testModeDirList.push_back(buf);
                    ++this->newDirCount;
                }
            } else {
                mkdir(buf.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // let the users umask handle the rest
                ++this->newDirCount;
            }
        }
    }
}

void DirEntryHandle::forkAndMove(const std::string &from, const std::string &to) {
    if (this->config.hasTestMode()) {
        this->testModeOutputQueue.push(from + std::string(" -> ") + to);
        ++this->movedFileCount;
        return;
    }

    // stack overflow to the rescue
    // http://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c
    int childExitStatus;
    pid_t pid = vfork();

    if (pid == 0) { /* child */
        execl("/bin/mv", "/bin/mv", from.c_str(), to.c_str(), (char *)NULL);
    } else if (pid < 0) {
        /* error - couldn't start process - you decide how to handle */
        ++this->forkFailCount;
    } else {
        /* parent - wait for child - this has all error handling, you
             * could just call wait() as long as you are only expecting to
             * have one child process at a time.
             */
//        pid_t ws = waitpid(pid, &childExitStatus, WNOHANG);
        pid_t ws = wait(&childExitStatus);

        if (ws == -1) {
            ++this->forkFailCount;
        }

        if (WIFEXITED(childExitStatus))
            /* exit code in childExitStatus */ {
            if (WEXITSTATUS(childExitStatus) == 0) {
                /* zero is normal exit */
                ++this->movedFileCount;
            } else {
                ++this->forkFailCount;
            }
        }
    }
}

bool DirEntryHandle::isDirectoryEmpty(const char *dirname) {
    // sometimes the program is too fast for IO to keep up.
    // To avoid seeing a directory that should be empty as nonempty,
    // we sync the buffers before checking for emptiness
    // because nothing is moved, it is senseless to sync in testmode
    if (!this->config.hasTestMode()) {
        sync();
    }

    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(dirname);

    if (dir == NULL) {
        //Not a directory or doesn't exist
        return 1;
    }

    while ((d = readdir(dir)) != NULL)
    {
        if(++n > 2) {
            break;
        }
    }
    closedir(dir);

    if (n <= 2) {
        //Directory Empty
        return true;
    } else {
        return false;
    }
}

std::string DirEntryHandle::getPathname(const std::string &path) {
    int pos = path.find_last_of('/');
    // what this function returns
    // /tmp/filename.mp3
    // ^^^^^
    return path.substr(0, pos);
}

std::string DirEntryHandle::getBasename(const std::string &path) {
    // what this function returns
    // /tmp/filename.mp3
    //      ^^^^^^^^
    int pos = path.find_last_of('/');
    return path.substr(pos+1, std::string::npos);
}

std::string DirEntryHandle::getSuffix(const std::string &path) {
    // what this function returns
    // /tmp/filename.mp3
    //              ^^^^
    int pos = path.find_last_of('.');
    return path.substr(pos, std::string::npos);
}

unsigned int DirEntryHandle::getMovedFileCount() const {
    return this->movedFileCount;
}

unsigned int DirEntryHandle::getNewDirCount() const {
    return this->newDirCount;
}

unsigned int DirEntryHandle::getDelDirCount() const {
    return this->removedDirCount;
}

std::queue<std::string> DirEntryHandle::getTestModeOutputQueue() {
    return this->testModeOutputQueue;
}
