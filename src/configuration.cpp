#include "configuration.h"

Configuration::Configuration()
{
    this->shouldSummarize = false;
    this->shouldTest      = false;
    this->shouldClean     = false;

    this->artist_regex_set = false;
    this->release_regex_set= false;
    this->title_regex_set  = false;
}

Configuration::~Configuration() {
    if (this->artist_regex_set)
        regfree(&this->artist_regex_buffer);

    if (this->release_regex_set)
        regfree(&this->release_regex_buffer);

    if (this->title_regex_set)
        regfree(&this->title_regex_buffer);
}

bool Configuration::isWalkRootValid() {
    bool blnRet;

    if (this->walkRoot.empty()) {
        blnRet = false;
    } else {
        blnRet = (access(this->walkRoot.c_str(), R_OK | W_OK | X_OK | F_OK) == 0);

        if (*(this->walkRoot.end()-1) == '/') {
            this->walkRoot = this->walkRoot.substr(0, walkRoot.length()-1);
        }
    }

    return blnRet;
}

bool Configuration::isPatternValid() {
    bool blnRet = true;

    if (this->mode == M_CHECK) {
        // CONTINUE HERE
        // something like this
        // -p "TYPE:REGEXP_MODS:'PATTERN' "
        // ex:
        // -p "a:i:(feat|www) t:i:\bfeat"

        regex_t pattern_buffer;
        size_t max_matches = 4;
        regmatch_t matches[max_matches];
        char pattern[] = "^([art]:[ie]{0,2}:.*){0,2}";

        // compile the pattern first
        if (regcomp(&pattern_buffer, pattern, REG_EXTENDED) == 0) {
            // and then use it on the pattern
            if (regexec(&pattern_buffer, this->pattern.c_str(), max_matches, matches, REG_NOTEOL) == 0 ) {
                // Index 0 of the match-array contains the whole match
                // so we'll only use indices 1 through 3
                for (unsigned int i = 1; i < max_matches && matches[i].rm_so != -1; ++i) {
                    blnRet = blnRet &&
                            this->handlePatternSection(this->pattern.substr(matches[i].rm_so,
                                                                            matches[i].rm_eo - matches[i].rm_so));
                }
            } else {
                // if regexec failed
                blnRet = false;
            }
            regfree(&pattern_buffer);
        } else {
            // if the regex failed to compile
            blnRet = false;
        }
    } else {
        blnRet = !this->pattern.empty();

        for (int i=0; blnRet && i<this->pattern.length(); ++i) {
            // check for the pattern initiator
            if (this->pattern.at(i) == '%') {
                // the letter following the initiator is the placeholder
                // test for a pattern that wants to crash the program first
                if (++i >= this->pattern.length()) {
                    blnRet = false;
                } else {
                    switch (this->pattern.at(i)) {
                    case 'a':
                    case 'r':
                    case 't': blnRet = true; break;
                    default: blnRet = false; break;
                    }
                }
            }
        }
    }

    // a filename must not contain any slashes
    if (this->mode == M_RENAME && this->pattern.find('/') != std::string::npos) {
        blnRet = false;
    }

    return blnRet;
}

bool Configuration::handlePatternSection(const std::string &part) {
    regex_t *buffer_ptr;
    int     flags = REG_NOSUB;

    // find out the type of tag the regex is for
    switch (part.at(0)) {
    case 'a':
        buffer_ptr = &this->artist_regex_buffer;
        this->artist_regex_set = true;
        break;

    case 'r':
        buffer_ptr = &this->release_regex_buffer;
        this->release_regex_set = true;
        break;

    case 't':
        buffer_ptr = &this->title_regex_buffer;
        this->title_regex_set = true;
        break;
    }

    // now find out what modifiers were set
    int i = 2;
    for (; part.at(i) != ':'; ++i) {
        switch (part.at(i)) {
        case 'e': flags |= REG_EXTENDED; break;
        case 'i': flags |= REG_ICASE;    break;
        }
    }

    // i is now at the index of the second colon
    // after that is the regex the user has set
    i++; //increment i so we can have the rest;

    return regcomp(buffer_ptr, part.substr(i).c_str(), flags) == 0;
}

void Configuration::setWalkRoot(const char *path) {
    this->walkRoot = std::string(path);
}

std::string Configuration::getWalkRoot() const{
    return this->walkRoot;
}

void Configuration::setPattern(const char *pat) {
    this->pattern = std::string(pat);
}

std::string Configuration::getPattern() const {
    return this->pattern;
}

void Configuration::setOpMode(OPMODE m) {
    this->mode = m;
}

Configuration::OPMODE Configuration::getOpMode() const {
    return this->mode;
}

void Configuration::setSummaryFlag(bool on) {
    this->shouldSummarize = on;
}

bool Configuration::hasSummaryFlag() const {
    return this->shouldSummarize;
}

void Configuration::setTestMode(bool on) {
    this->shouldTest = on;
}

bool Configuration::hasTestMode() const {
    return this->shouldTest;
}

void Configuration::setCleanup(bool on) {
    this->shouldClean = on;
}

bool Configuration::shouldCleanup() const {
    return this->shouldClean;
}

const regex_t* Configuration::getArtistRegexBufferPtr() const {
    return &this->artist_regex_buffer;
}

const regex_t* Configuration::getReleaseRegexBufferPtr() const {
    return &this->release_regex_buffer;
}

const regex_t* Configuration::getTitleRegexBufferPtr() const {
    return &this->title_regex_buffer;
}
