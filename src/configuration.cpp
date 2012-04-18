#include "configuration.h"

Configuration::Configuration()
{
    this->isVerbose       = false;
    this->shouldSummarize = false;
    this->shouldTest      = false;
}

bool Configuration::isWalkRootValid() {
    bool blnRet;

    if (this->walkRoot.empty()) {
        blnRet = false;
    } else {
        blnRet = (access(this->walkRoot.c_str(), R_OK | W_OK | X_OK | F_OK) == 0);
    }

    return blnRet;
}

bool Configuration::isPatternValid() {
    bool blnRet = !this->pattern.empty();

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

    return blnRet;
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

void Configuration::setVerbose(bool verb) {
    this->isVerbose = verb;
}

bool Configuration::hasVerbose() const {
    return this->isVerbose;
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
