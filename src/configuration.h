#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <unistd.h>
#include <regex.h>
#include <sys/types.h>


#include <string>

/* This class provides an easy interface for querying
 * the program options that have been set on the commandline
 */
class Configuration {
public:
    Configuration();
    ~Configuration();

    enum OPMODE { M_REORDER, M_RENAME, M_CHECK };

    // isWalkRootValid determines wether the given root
    // is accessible and wether we have the right permissions
    // to create directories and move stuff around
    bool isWalkRootValid();

    // isPatternValid determines whether the given pattern
    // is valid and that it can be expanded if given enough data
    // if in check mode, this function splits the given pattern
    // into the different sections (album, title, artist)
    // and checks their validity
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

    // buffer boolean and pointer getters
    bool hasArtistRegexBuffer() const;
    bool hasReleaseRegexBuffer() const;
    bool hasTitleRegexBuffer() const;

    const regex_t* getArtistRegexBufferPtr() const;
    const regex_t* getReleaseRegexBufferPtr() const;
    const regex_t* getTitleRegexBufferPtr() const;

private:
    // only for checkmode!
    int countPatternSections();

    bool handlePatternSection(const std::string &part);

private:
    std::string walkRoot;
    std::string pattern;

    // compiled regexes
    regex_t artist_regex_buffer;
    regex_t release_regex_buffer;
    regex_t title_regex_buffer;
    // indicators
    bool artist_regex_set;
    bool release_regex_set;
    bool title_regex_set;

    OPMODE mode;

    bool shouldSummarize;
    bool shouldTest;
    bool shouldClean;
};

#endif // CONFIGURATION_H
