#include "walker.h"

Walker::Walker(const Configuration &cfg) : config(cfg), deHandle(cfg) {
    this->unableToHandleCount = 0;
}

int Walker::handleDirEntry(const char *fpath, const struct stat *sb, int tflag, FTW *ftwbuf) {
    if (tflag == FTW_F) {
        TagLib::FileRef fileref(fpath, false);
        std::string     exPattern;

        // see whether the file is good for working with
        if (!fileref.isNull() && fileref.tag() != NULL) {
            if (this->config.getOpMode() == Configuration::M_CHECK) {
            } else if (this->expandPattern(fileref.tag(), exPattern)) {

                // we have enough tag info - so let's begin

                switch (this->config.getOpMode()) {
                case Configuration::M_REORDER:
                    this->deHandle.handleReorderMode(exPattern, fpath);
                    break;

                case Configuration::M_RENAME:
                    this->deHandle.handleRenameMode(exPattern, fpath);
                    break;
                }
            } else {
                // expansion failed, put that into the statistics
                ++this->unableToHandleCount;
            }
        } else if (tflag == FTW_D) {
            this->deHandle.handleDirectory(fpath);
        }
    }

    return 0;
}

bool Walker::expandPattern(const TagLib::Tag *tr, std::string &expansion_str) {
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
    // normally artists don't use percentage signs in their names...YET
    // TODO: find a better pattern initiator
    return (expansion_str.find('%') == std::string::npos);
}

unsigned int Walker::getMovedFileCount() const {
    return this->deHandle.getMovedFileCount();
}

unsigned int Walker::getNewDirCount() const {
    return this->deHandle.getNewDirCount();
}

unsigned int Walker::getDelDirCount() const {
    return this->deHandle.getDelDirCount();
}

unsigned int Walker::getUnableToHandleCount() const {
    return this->unableToHandleCount;
}

std::queue<std::string> Walker::getTestOutputQueue() {
    return this->deHandle.getTestModeOutputQueue();
}
