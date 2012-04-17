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

    enum OPMODE { M_REORDER };

    // isWalkRootValid determines wether the given root
    // is accessible and wether we have the right permissions
    // to create directories and move stuff around
    bool isWalkRootValid();

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

    // isVerbose get an set methods
    void setVerbose(bool verb);
    bool hasVerbose() const;

    // shouldSummarize get an set methods
    void setSummaryFlag(bool on);
    bool hasSummaryFlag() const;

    // shouldTest get and set methods
    void setTestMode(bool on);
    bool hasTestMode() const;

private:
    std::string walkRoot;
    std::string pattern;

    OPMODE mode;

    bool isVerbose;
    bool shouldSummarize;
    bool shouldTest;
};

#endif // CONFIGURATION_H
