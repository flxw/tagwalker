#include "walker.h"
#include <iostream>

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

                int checkRes = this->deHandle.handleCheckMode(
                            fileref.tag()->artist().toCString(true),
                            fileref.tag()->album().toCString(true),
                            fileref.tag()->title().toCString(true));

                if (this->actOnCheckResult(checkRes, fileref)) {
                    fileref.save();
                }

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
        }
    } else if (tflag == FTW_D) {
        this->deHandle.handleDirectory(fpath);
    }

    return 0;
}

bool Walker::actOnCheckResult(int cr, const TagLib::FileRef &fr) {
    bool modified = false;

    if (cr != DirEntryHandle::CR_OKAY) {
        if ((cr & DirEntryHandle::CR_ARTIST) == DirEntryHandle::CR_ARTIST) {
            // the album regex matched, so act accordingly
            std::string originalTag = fr.tag()->artist().to8Bit(true);
            std::string artistEdit  = UserInterface::editArtistTagOnPrompt(originalTag.c_str());

            if (originalTag != artistEdit) {
                fr.tag()->setArtist(artistEdit);
                modified = true;
            }
        }

        if ((cr & DirEntryHandle::CR_RELEASE) == DirEntryHandle::CR_RELEASE) {
            // the album regex matched, so act accordingly
            std::string originalTag = fr.tag()->album().to8Bit(true);
            std::string releaseEdit = UserInterface::editArtistTagOnPrompt(originalTag.c_str());

            if (originalTag != releaseEdit) {
                fr.tag()->setAlbum(releaseEdit);
                modified = true;
            }
        }

        if ((cr & DirEntryHandle::CR_TITLE) == DirEntryHandle::CR_TITLE) {
            // the album regex matched, so act accordingly
            std::string originalTag = fr.tag()->title().to8Bit(true);
            std::string titleEdit = UserInterface::editArtistTagOnPrompt(originalTag.c_str());

            if (originalTag != titleEdit) {
                fr.tag()->setTitle(titleEdit);
                modified = true;
            }
        }
    }

    return modified;
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
