#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <unistd.h>

#include <string>

/* This class provides an easy interface for querying
 * the program options that have been set on the commandline
 */
class Configuration {
public:
    Configuration();

    enum OPMODE { M_REORDER, M_RENAME };

    // isWalkRootValid determines wether the given root
    // is accessible and wether we have the right permissions
    // to create directories and move stuff around
    bool isWalkRootValid();

    // isPatternValid determines whether the given pattern
    // is valid and that it can be expanded given enough data
    bool isPatternValid();

    /* GETTERS AND SETTERS FROM HERE ON */
    // walkRoot get and set methods
    void setWalkRoot(const char* path);
    std::string getWalkRoot(void) const;

    // pattern get and set methods
    void setPattern(const char* pat);
    std::string getPattern(void) const;

    // mode get and set methods
    void setOpMode(OPMODE m);
    OPMODE getOpMode() const;

    // shouldSummarize get an set methods
    void setSummaryFlag(bool on);
    bool hasSummaryFlag() const;

    // shouldTest get and set methods
    void setTestMode(bool on);
    bool hasTestMode() const;

    // shouldCleanup get and set methods
    void setCleanup(bool on);
    bool shouldCleanup() const;

private:
    std::string walkRoot;
    std::string pattern;

    OPMODE mode;

    bool shouldSummarize;
    bool shouldTest;
    bool shouldClean;
};

#endif // CONFIGURATION_H
