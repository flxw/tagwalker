#include "tagwalker.h"

TagWalker::TagWalker(const Configuration &conf) : config(conf)
{
    this->movedFileCount      = 0;
    this->newDirCount         = 0;
    this->unableToHandleCount = 0;
    this->removedDirCount     = 0;
}

int TagWalker::handleDirEntry(const char *fpath, const struct stat *sb, int tflag, FTW *ftwbuf) {
    // identify what fpath points to
    if (tflag == FTW_F) {
        TagLib::FileRef file(fpath, false);

        // see wether the file is good for working with
        if (!file.isNull() && !file.tag()->isEmpty()) {
            // TODO: check tag sufficiency
            // we have *some* tags - so let's get crackin'
            switch (this->config.getOpMode()) {
                case Configuration::M_REORDER:
                    this->handleReorderMode(file, std::string(fpath));
                    break;
                case Configuration::M_RENAME:
                    this->handleRenameMode(file, std::string(fpath));
                    break;
            default: break;
            }
        }
    } else if (tflag == FTW_DP) {
        if (this->config.shouldCleanup() && this->isDirectoryEmpty(fpath)) {
            if (this->config.hasTestMode()) {
                this->testModeOutputQueue.push(std::string("Delete `") + std::string(fpath) + std::string("'"));
            } else {
                rmdir(fpath);
            }
            ++removedDirCount;
        }
    }
    return 0;
}

void TagWalker::handleReorderMode(const TagLib::FileRef &fr, std::string path) {
    // check whether the file is already in the right place
    // to do that, expand the pattern first and make it a complete filename
    std::string expandedPattern;

    if (this->expandPattern(fr.tag(), expandedPattern)) {

        // put the pieces together - depending what opmode we have
        expandedPattern = this->config.getWalkRoot() + expandedPattern;
        expandedPattern.append(this->getBasename(path));

        // only act if the two directories differ
        if ( path != expandedPattern ) {
            this->RecursivelyMkdir(expandedPattern);
            this->forkAndMove(path, expandedPattern);
        }
    } else {
        ++this->unableToHandleCount;
    }
}

void TagWalker::handleRenameMode(const TagLib::FileRef &fr, std::string path) {
    // check whether the file is already in the right place
    // to do that, expand the pattern first and make it a complete filename
    std::string expandedPattern;

    if (this->expandPattern(fr.tag(), expandedPattern)) {

        expandedPattern = this->getPathname(path) + std::string("/") + expandedPattern;
        expandedPattern.append(this->getSuffix(path));

        // only act if the two directories differ
        if ( path != expandedPattern ) {
            this->forkAndMove(path, expandedPattern);
        }
    } else {
        ++this->unableToHandleCount;
    }
}

bool TagWalker::expandPattern(const TagLib::Tag *tr, std::string &expansion_str) {
    expansion_str = this->config.getPattern();

    // Expand the pattern with the information contained in tr
    for (unsigned int i=0; i<expansion_str.size(); ++i) {
        // seek for a pattern initializer
        if (expansion_str.at(i) == '%') {
            // %a - artist
            if (expansion_str.at(i+1) == 'a' && !tr->artist().isEmpty()) {
                expansion_str.replace(i, 2, tr->artist().to8Bit(true));
                i += tr->artist().length()-1; //update the index
            }
            // %r - release/album
            else if (expansion_str.at(i+1) == 'r' && !tr->album().isEmpty()) {
                expansion_str.replace(i, 2, tr->album().to8Bit(true));
                i += tr->artist().length()-1; //update the index
            }
            // %t - track
            else if (expansion_str.at(i+1) == 't' && !tr->title().isEmpty()) {
                expansion_str.replace(i, 2, tr->title().to8Bit(true));
                i += tr->title().length()-1;
            }
        }
    }


    if (this->config.getOpMode() == Configuration::M_REORDER) {
        // check for preceding and following slashes
        if (*(expansion_str.begin()) != '/') {
            expansion_str = "/" + expansion_str;
        }

        if (*(expansion_str.end()-1) != '/') {
            expansion_str.append("/");
        }
    }

    // finally return a boolean indicating wether expansion was successful
    return (expansion_str.find('%') == std::string::npos);
}

void TagWalker::RecursivelyMkdir(const std::string &path) {
    std::string p = this->getPathname(path);
    unsigned int i = p.find('/', this->config.getWalkRoot().length()+1);

    // Test for directory existence first
    // TODO put that in testModeDirList as well?!
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

void TagWalker::forkAndMove(const std::string &from, const std::string &to) {
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
        ++this->unableToHandleCount;
    } else {
        /* parent - wait for child - this has all error handling, you
             * could just call wait() as long as you are only expecting to
             * have one child process at a time.
             */
//        pid_t ws = waitpid(pid, &childExitStatus, WNOHANG);
        pid_t ws = wait(&childExitStatus);

        if (ws == -1) {
            ++this->unableToHandleCount;
        }

        if (WIFEXITED(childExitStatus))
            /* exit code in childExitStatus */ {
            if (WEXITSTATUS(childExitStatus) == 0) {
                /* zero is normal exit */
                ++this->movedFileCount;
            } else {
                ++this->unableToHandleCount;
            }
        }
    }
}

bool TagWalker::isDirectoryEmpty(const char *dirname) {
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

std::string TagWalker::getPathname(const std::string &path) {
    int pos = path.find_last_of('/');
    // what this function returns
    // /tmp/filename.mp3
    // ^^^^^
    return path.substr(0, pos);
}

std::string TagWalker::getBasename(const std::string &path) {
    // what this function returns
    // /tmp/filename.mp3
    //      ^^^^^^^^
    int pos = path.find_last_of('/');
    return path.substr(pos+1, std::string::npos);
}

std::string TagWalker::getSuffix(const std::string &path) {
    // what this function returns
    // /tmp/filename.mp3
    //              ^^^^
    int pos = path.find_last_of('.');
    return path.substr(pos, std::string::npos);
}

unsigned int TagWalker::getMovedFileCount() const {
    return this->movedFileCount;
}

unsigned int TagWalker::getNewDirCount() const {
    return this->newDirCount;
}

unsigned int TagWalker::getDelDirCount() const {
    return this->removedDirCount;
}

unsigned int TagWalker::getNoHandleCount() const {
    return this->unableToHandleCount;
}

std::queue<std::string> TagWalker::getTestModeOutputQueue() {
    return this->testModeOutputQueue;
}
