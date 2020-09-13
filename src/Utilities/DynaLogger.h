/**
 * @file DynaLogger.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2019-12-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef DYNALOGGER_H
#define DYNALOGGER_H

#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

#include "String.h"
#include "IntWrapper.h"
#include "DynaHashMap.h"
#include "DynaHashSet.h"
#include "FileUtils.h"
//#include "concurrent/concurrent_queue.h"

/**
 * @brief DynaLogger - simple, small, powerful logging system
 * 
 * Manager
 * 
 * There is a single DynaLoggerManager singleton.  This maintains a hash of all loggers, and
 * also has a master log level range (min and max).  This range is used for all newly added
 * loggers.  If a logger does not set its own level range, it will use the master range.
 * 
 * Loggers
 * 
 * Each section of a system can have its own DynaLogger.  Each logger can be configured with
 * a log level range (min and max).  For each appender added to a logger, it will either use
 * the levels of its parent logger, or it will have specific overrides.
 * 
 * Appenders
 * 
 * Every logger has one or more appenders, which has a reference to one of the output sinks
 * held by the DynaLoggerManager, and its own log levels (min and max).
 * 
 * Output Sinks
 * 
 * An output sink is either a device, such as Console or Error display, or it is a file.  At some
 * point we may add things like Sockets.  An output sink can have it's own range of log levels,
 * thereby preventing any log messages getting to it outside of this range.  Since all appenders
 * from all loggers share the same output sinks, this is a firm way to control what messages
 * get to each sink.
 * 
 */

class DynaOutputSink;
class DynaOutputSinkFile;
class DynaLogger;
class DynaAppender;

/**
 * @brief From coarsest to finest setting.  FATAL produces the least output, while TRACE
 * produces the most. 
 */
enum LogLevel{NONE, FATAL, CRITICAL, ERROR, WARNING, INFO, DEBUG, TRACE, ALL};
enum SinkType{FILE_SINK, CONSOLE_SINK, ERROR_SINK};

enum ANSIColors{NO_COLOR, DARK_BG, LIGHT_BG};

static const int MAX_ROLLING_KEEP     = 20;
static const int MIN_MAX_FILE_SIZE_KB = 50;
static const int MAX_LOG_ENTRY_SIZE   = 100000;

static const uint16_t TRIGGER_DATE_CHANGE    = 0x0001U;
static const uint16_t TRIGGER_HOUR_CHANGE    = 0x0002U;
static const uint16_t ROLLING_ARCHIVE        = 0x0004U;

static const char* sinkTypeName[] {"file", "console", "error"};
static const char* logLevelName[] {"NONE", "FATAL", "CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE", "ALL"};

typedef IntWrapper<SinkType> ESinkType;

MAKE_MAPTYPE_DEF(String, DynaLogger, DynaLogger);
MAKE_MAPTYPE_DEF(ESinkType, DynaOutputSink, DynaOutputType);
MAKE_MAPTYPE_DEF(String, DynaOutputSink, DynaOutputFile);
MAKE_MAPTYPE_DEF(ESinkType, DynaAppender, DynaAppenderType);
MAKE_MAPTYPE_DEF(String, DynaAppender, DynaAppenderFile);
MAKE_SETTYPE_DEF(DynaOutputSink, DefaultSink);

class DynaLoggerManager {
    static DynaLogger*        logger;
    static DynaLoggerManager* _instance;
    DynaLoggerMap*      _loggerMap;
    DynaOutputTypeMap*  _outputTypeMap;
    DynaOutputFileMap*  _outputFileMap;
    DefaultSinkSet*     _defSinkSet;
    LogLevel            _minLogLevel;
    LogLevel            _maxLogLevel;
    String              _rootLogDirectory;
    bool                _autoFlushAll;
    bool                _disableColours;

    friend class DynaOutputSink;

    DynaLoggerManager();

public:
    static DynaLoggerManager*   getSingleton();
    static DynaLogger*          getManagerLogger();
    static void                 deleteSingleton();
    static DynaOutputSinkFile*  initDefaultSink(const char* fileNamePrefix);

    virtual ~DynaLoggerManager();

    void        shutdown();
    String&     getRootLogDirectory();
    bool        setRootLogDirectory(const char* directory);
    DynaLogger* getLogger(const char* name);
    bool        registerLogger(const char* name, DynaLogger* logger);
    void        flushAll();
    void        setAutoFlushAll(bool autoFlushAll);
    bool        isAutoFlushAll();
    void        setDisableColours(bool disableColours);
    bool        isDisableColours();

    DynaOutputSink*     addOutputSink(DynaOutputSink* sink);
    DynaOutputSink*     addOutputSink(SinkType type);
    DynaOutputSinkFile* addOutputSink(const char* fileNamePrefix);
    bool                enableOutputSink(SinkType type, bool enable);
    bool                enableOutputSink(const char* fileNamePrefix, bool enable);
    // bool                removeOutputSink(SinkType type);
    // bool                removeOutputSink(const char* fileNamePrefix);

    DynaOutputSink*     getOutputSink(SinkType type);
    DynaOutputSinkFile* getOutputSink(const char* fileNamePrefix);

    DynaOutputSink*     addDefaultSink(SinkType type);
    DynaOutputSinkFile* addDefaultSink(const char* fileNamePrefix);
    bool                removeDefaultSink(SinkType type);
    bool                removeDefaultSink(const char* fileNamePrefix);
    DefaultSinkSet*     getDefaultSinks();
    void                initAllLoggers();

    LogLevel getMinLogLevel();
    LogLevel getMaxLogLevel();
    void     setLogLevel(LogLevel level, bool resetOverride = false);
    void     setMinLogLevel(LogLevel minLevel, bool resetOverride = false);
    void     setMaxLogLevel(LogLevel maxLevel, bool resetOverride = false);
    void     setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool resetOverride = false);
    /**
     * @brief Set/Override the levels of a specified logger
     * 
     * @param loggerName 
     * @param minLevel 
     * @param maxLevel 
     */
    void     setLoggerLogLevels(const char* loggerName, LogLevel minLevel, LogLevel maxLevel, bool resetOverride = false);
    void     setOutputLogLevels(SinkType type, LogLevel minLevel, LogLevel maxLevel, bool resetOverride = false);
    void     setOutputLogLevels(const char* filePrefixName, LogLevel minLevel, LogLevel maxLevel, bool resetOverride = false);

    /**
     * @brief Reset the levels of the specified logger so they match the manager levels
     * 
     * @param name 
     * @param includeAppenders 
     */
    void        updateLoggerLogLevels(const char* name, bool resetOverride = false);
    void        updateAllLogLevels(bool resetOverride = false);
};

class DynaOutputSink : public IHashable<DynaOutputSink>, public ICopyable<DynaOutputSink> {
protected:
    SinkType          _type;
    LogLevel          _minLogLevel;
    LogLevel          _maxLogLevel;
    bool              _levelOverride;
    ANSIColors        _colors;
    bool              _autoFlush;
    bool              _enabled;

protected:
    DynaLoggerManager* _mgr;
//    ste::concurrent_queue<String*>* _queue;

public:
    DynaOutputSink();
    DynaOutputSink(SinkType type);
    virtual ~DynaOutputSink();

    DynaOutputSink(const DynaOutputSink& other);
    DynaOutputSink* copy() override;

    //-----------------------------------------------------------------
    //                   IHashable Implementation
    //-----------------------------------------------------------------
    int  hashCode() const override;
    bool operator== (const DynaOutputSink& other) const override;

    SinkType getType();

    void     setManager(DynaLoggerManager* mgr);

    void     setAnsiColors(ANSIColors colors);
    void     setAutoFlush(bool autoFlush);
    bool     isEnabled();
    void     setEnabled(bool enabled);

    LogLevel getMinLogLevel();
    LogLevel getMaxLogLevel();
    void     setLogLevel(LogLevel level);
    void     setMinLogLevel(LogLevel minLevel);
    void     setMaxLogLevel(LogLevel maxLevel);
    void     setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool override = true);

    /**
     * @brief Get the ANSI Color Tag object
     * 
     *            Foregroud  Background
     * black        30         40
     * red          31         41
     * green        32         42
     * yellow       33         43
     * blue         34         44
     * magenta      35         45
     * cyan         36         46
     * white        37         47
     * 
     * reset             0  (everything back to normal)
     * bold/bright       1  (often a brighter shade of the same colour)
     * underline         4
     * inverse           7  (swap foreground and background colours)
     * bold/bright off  21
     * underline off    24
     * inverse off      27
     * 
     * @param level 
     * @return String 
     */
    String   ansiOn(LogLevel level);
    bool     isLevelOverride();

    virtual bool appendToQueue(LogLevel level, const char* outStr);
    virtual void flush();
};

class DynaOutputSinkConsole : public DynaOutputSink {
public:
    DynaOutputSinkConsole();
    DynaOutputSinkConsole(const char* fileNamePrefix);
    virtual ~DynaOutputSinkConsole();

    DynaOutputSinkConsole(const DynaOutputSinkConsole& other);
    DynaOutputSinkConsole* copy() override;

    bool appendToQueue(LogLevel level, const char* outStr) override;
    void flush() override;
};

class DynaOutputSinkError : public DynaOutputSink {

public:
    DynaOutputSinkError();
    DynaOutputSinkError(const char* fileNamePrefix);
    virtual ~DynaOutputSinkError();

    DynaOutputSinkError(const DynaOutputSinkError& other);
    DynaOutputSinkError* copy() override;

    bool appendToQueue(LogLevel level, const char* outStr) override;
    void flush() override;
};

class DynaOutputSinkFile : public DynaOutputSink {
    String     _fileNamePrefix;
    String     _fileName;
    FILE*      _logFile;
    uint16_t   _filePolicies;
    uint64_t   _maxFileSize;    // bytes
    int        _maxFiles;
    int        _curDayOfYear;
    int        _curHourOfDay;

public:
    DynaOutputSinkFile(const char* fileNamePrefix);
    virtual ~DynaOutputSinkFile();

    DynaOutputSinkFile(const DynaOutputSinkFile& other);
    DynaOutputSinkFile* copy() override;

    //-----------------------------------------------------------------
    //                   IHashable Overrides
    //-----------------------------------------------------------------
    int    hashCode() const override;

    String& getFileNamePrefix();
    String& getFileName();
    FILE*   getFile();

    void     setPolicies(uint16_t policyBits);
    void     clearPolicies(uint16_t policyBits);
    void     setMaxFileSizeKB(uint32_t maxSize);
    uint64_t getMaxFileSizeKB();
    void     setRollingMaxKeep(int maxKeepFiles);
    int      getRollingMaxKeep();

    bool    open();
    bool    isOpen();
    void    close();
    tm      getCurrentTimeInfo();
    bool    fileNeedsToChange();
    void    shuffleFileNames(String& fileName, int index);
    void    getNextFileName();

    bool appendToQueue(LogLevel level, const char* outStr) override;
    void flush() override;
};


class DynaLogger : public IHashable<DynaLogger>, public ICopyable<DynaLogger> {
    String               _name;
    DynaAppenderTypeMap* _appenderTypeMap;
    DynaAppenderFileMap* _appenderFileMap;
    LogLevel             _minLogLevel;
    LogLevel             _maxLogLevel;
    bool                 _levelOverride;
    long                 _traceCount;
    long                 _traceCounter;
    bool                 _compactConsoleDebug;

    DynaLogger();

    system_clock::time_point _getNow();
    String _getDateTime(system_clock::time_point t1, const char* format);
    void _log(LogLevel level, const char* msg, va_list args);

public:
    virtual ~DynaLogger();

    DynaLogger(const DynaLogger& other);
    DynaLogger* copy() override;

    //-----------------------------------------------------------------
    //                   IHashable Implementation
    //-----------------------------------------------------------------
    int  hashCode() const override;
    bool operator== (const DynaLogger& other) const override;

    static DynaLogger* getLogger(const char* name, bool addDefAppenders = true);

    void addDefaultAppenders();
    void setTraceCount(long count);
    long getTraceCount();
    void resetTraceCounter();
    void setCompactConsoleDebug(bool compactOn);

    DynaAppender* addAppender(DynaAppender* appender);
    DynaAppender* addAppender(SinkType type);
    DynaAppender* addAppender(const char* fileNamePrefix);
    DynaAppender* getAppender(SinkType type);
    DynaAppender* getAppender(const char* fileNamePrefix);
    bool          removeAppender(SinkType type);
    bool          removeAppender(const char* fileNamePrefix);

    LogLevel getMinLogLevel();
    LogLevel getMaxLogLevel();
    void     setLogLevel(LogLevel level);
    void     setMinLogLevel(LogLevel minLevel);
    void     setMaxLogLevel(LogLevel maxLevel);
    void     setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool override = true);
    void     setAppenderLogLevels(SinkType type, LogLevel minLevel, LogLevel maxLevel);
    void     setAppenderLogLevels(const char* filePrefixName, LogLevel minLevel, LogLevel maxLevel);

    bool     isLevelOverride();

    void     updateAppenderLogLevels(SinkType type, bool resetOverride = false);
    void     updateAppenderLogLevels(const char* fileNamePrefix, bool resetOverride = false);
    void     updateAllAppenderLogLevels(bool resetOverride = false);

    void log(LogLevel level, const char* msg, ...);
    void fatal(const char* msg, ...);
    void critical(const char* msg, ...);
    void error(const char* msg, ...);
    void warning(const char* msg, ...);
    void info(const char* msg, ...);
    void debug(const char* msg, ...);
    void trace(const char* msg, ...);
};

class DynaAppender : public IHashable<DynaAppender>, public ICopyable<DynaAppender> {
protected:
    SinkType        _type;
    DynaOutputSink* _output;
    LogLevel        _minLogLevel;
    LogLevel        _maxLogLevel;
    bool            _levelOverride;

    DynaAppender(SinkType type);

public:
    static DynaAppender* getTypedAppender(SinkType sinkType);
    static DynaAppender* getConsoleAppender();
    static DynaAppender* getErrorAppender();
    static DynaAppender* getFileAppender(const char* fileNamePrefix);
    virtual ~DynaAppender();

    DynaAppender(const DynaAppender& other);
    DynaAppender* copy() override;

    //-----------------------------------------------------------------
    //                   IHashable Implementation
    //-----------------------------------------------------------------
    int  hashCode() const override;
    bool operator== (const DynaAppender& other) const override;

    LogLevel getMinLogLevel();
    LogLevel getMaxLogLevel();
    void     setLogLevel(LogLevel level);
    void     setMinLogLevel(LogLevel minLevel);
    void     setMaxLogLevel(LogLevel maxLevel);
    void     setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool override = true);

    bool     isLevelOverride();

    DynaOutputSink* getOutputSink();
    void            setOutputSink(DynaOutputSink* sink);
    SinkType        getType();

    bool append(LogLevel level, const char* outStr);
};
#endif
