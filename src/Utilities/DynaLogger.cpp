/**
 * @file DynaLogger.cpp
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2019-12-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Utils.h"
#include "DynaHashMapImpl.h"
#include "DynaHashSetImpl.h"
#include "DynaLogger.h"
#include "TimeUtils.h"

MAKE_MAPTYPE_INSTANCE(String, DynaLogger, DynaLogger);
MAKE_MAPTYPE_INSTANCE(ESinkType, DynaOutputSink, DynaOutputType);
MAKE_MAPTYPE_INSTANCE(String, DynaOutputSink, DynaOutputFile);
MAKE_MAPTYPE_INSTANCE(ESinkType, DynaAppender, DynaAppenderType);
MAKE_MAPTYPE_INSTANCE(String, DynaAppender, DynaAppenderFile);
MAKE_SETTYPE_INSTANCE(DynaOutputSink, DefaultSink);

//template class ste::concurrent_queue<String>;

DynaLoggerManager* DynaLoggerManager::_instance = nullptr;
DynaLogger* DynaLoggerManager::logger = DynaLogger::getLogger("LogManager");

DynaLoggerManager::DynaLoggerManager() : _minLogLevel(NONE), _maxLogLevel(ALL), _autoFlushAll(false) {
    _loggerMap      = new DynaLoggerMap();
    _outputTypeMap  = new DynaOutputTypeMap();
    _outputFileMap  = new DynaOutputFileMap();
    _defSinkSet     = new DefaultSinkSet();
    _defSinkSet->setOwnsMembers(false);     // This set does not own the sinks put into it
    addDefaultSink(CONSOLE_SINK);
    addDefaultSink(ERROR_SINK);
    setRootLogDirectory("logs");
}

DynaLoggerManager::~DynaLoggerManager() {
    shutdown();
}

DynaLogger* DynaLoggerManager::getManagerLogger() {
    return logger;
}

DynaLoggerManager* DynaLoggerManager::getSingleton() {
    if (_instance  == nullptr) {
        _instance = new DynaLoggerManager();
    }
    return _instance;
}

void DynaLoggerManager::deleteSingleton() {
    if (_instance != nullptr) {
        delete _instance;
        _instance = nullptr;
    }
}

DynaOutputSinkFile* DynaLoggerManager::initDefaultSink(const char* fileNamePrefix) {
    DynaLoggerManager* manager = DynaLoggerManager::getSingleton();
    DynaOutputSinkFile* sink   = manager->addDefaultSink(fileNamePrefix);
    return sink;
}

void DynaLoggerManager::shutdown() {
    delete _loggerMap;
    _loggerMap = nullptr;
    delete _outputTypeMap;
    _outputTypeMap = nullptr;
    delete _outputFileMap;
    _outputFileMap = nullptr;
    delete _defSinkSet;
    _defSinkSet = nullptr;
}

using namespace std;

bool DynaLoggerManager::setRootLogDirectory(const char* directory) {
    String outStr;
    if (FileUtils::ensureDirectory(directory, outStr)) {
        _rootLogDirectory = outStr;
        return true;
    }
    return false;
}

DynaLogger* DynaLoggerManager::getLogger(const char* name) {
    auto temp = String(name);
    return _loggerMap->get(&temp);
}

bool DynaLoggerManager::registerLogger(const char* name, DynaLogger* logger) {
    auto temp = String(name);
    if (!_loggerMap->containsKey(&temp)) {
        // Default the logger's levels to match the current main levels...
        logger->setLogLevels(_minLogLevel, _maxLogLevel, false);
        _loggerMap->put(new String(name), logger);
        return true;
    }
    return false;
}

void DynaLoggerManager::flushAll() {
    for (DynaOutputSink* sink : _outputTypeMap->values()) {
        sink->flush();
    }
    for (DynaOutputSink* sink : _outputFileMap->values()) {
        sink->flush();
    }
}

void DynaLoggerManager::setAutoFlushAll(bool autoFlushAll) {
    _autoFlushAll = autoFlushAll;
}

bool DynaLoggerManager::isAutoFlushAll() {
    return _autoFlushAll;
}

void DynaLoggerManager::setDisableColours(bool disableColours) {
    _disableColours = disableColours;
}

bool DynaLoggerManager::isDisableColours() {
    return _disableColours;
}

DynaOutputSink* DynaLoggerManager::addOutputSink(DynaOutputSink* sink) {
    if (sink->getType() != FILE_SINK) {
        auto sinkType = ESinkType(sink->getType());
        if (!_outputTypeMap->containsKey(&sinkType)) {
            _outputTypeMap->put(new ESinkType(sinkType), sink);
        }
    }
    else {
        auto prefix = ((DynaOutputSinkFile*)sink)->getFileNamePrefix();
        if (!_outputFileMap->containsKey(&prefix)) {
            _outputFileMap->put(prefix, sink);
        }
    }
    if (_autoFlushAll) {
        sink->setAutoFlush(true);
    }
    sink->setManager(this);
    return sink;
}

DynaOutputSink* DynaLoggerManager::addOutputSink(SinkType type) {
    auto* sink = getOutputSink(type);
    if (sink == nullptr) {
        switch(type) {
            case CONSOLE_SINK:
                sink = new DynaOutputSinkConsole();
                break;
            case ERROR_SINK:
                sink = new DynaOutputSinkError();
                break;
            default:
                break;
        }
        if (sink != nullptr) {
            sink = addOutputSink(sink);
        }
    }
    sink->setManager(this);
    return sink;
}

DynaOutputSinkFile* DynaLoggerManager::addOutputSink(const char* fileNamePrefix) {
    auto* sink = getOutputSink(fileNamePrefix);
    if (sink == nullptr) {
        sink = (DynaOutputSinkFile*)addOutputSink(new DynaOutputSinkFile(fileNamePrefix));
    }
    sink->setManager(this);
    return sink;
}

bool DynaLoggerManager::enableOutputSink(SinkType type, bool enable) {
    auto* sink = getOutputSink(type);
    if (sink != nullptr) {
        sink->setEnabled(enable);
        return true;
    }
    return false;
}

bool DynaLoggerManager::enableOutputSink(const char* fileNamePrefix, bool enable) {
    auto* sink = getOutputSink(fileNamePrefix);
    if (sink != nullptr) {
        sink->setEnabled(enable);
        return true;
    }
    return false;
}


// bool DynaLoggerManager::removeOutputSink(SinkType type) {
//     auto sinkType = ESinkType(type);
//     if (_outputTypeMap->containsKey(&sinkType)) {
//         _outputTypeMap->deleteEntry(sinkType);
//         return true;
//     }
//     return false;
// }

// bool DynaLoggerManager::removeOutputSink(const char* fileNamePrefix) {
//     if (_outputFileMap->containsKey(fileNamePrefix)) {
//         auto* sink = getOutputSink(fileNamePrefix);
//         _defSinkSet->deleteEntry(sink);
//         return true;
//     }
//     return false;
// }

DynaOutputSink* DynaLoggerManager::getOutputSink(SinkType type) {
    return _outputTypeMap->get(type);
}

DynaOutputSinkFile* DynaLoggerManager::getOutputSink(const char* fileNamePrefix) {
    return (DynaOutputSinkFile*)_outputFileMap->get(fileNamePrefix);
}

DynaOutputSink* DynaLoggerManager::addDefaultSink(SinkType type) {
    auto* sink = addOutputSink(type);
    if (sink != nullptr && !_defSinkSet->contains(sink)) {
        _defSinkSet->add(sink);
    }
    sink->setManager(this);
    return sink;
}

DynaOutputSinkFile* DynaLoggerManager::addDefaultSink(const char* fileNamePrefix) {
    auto* sink = addOutputSink(fileNamePrefix);
    if (sink != nullptr && !_defSinkSet->contains(sink)) {
        _defSinkSet->add(sink);
    }
    sink->setManager(this);
    return sink;
}

bool DynaLoggerManager::removeDefaultSink(SinkType type) {
    auto* sink = getOutputSink(type);
    if (sink != nullptr && _defSinkSet->contains(sink)) {
        _defSinkSet->deleteEntry(sink);
        return true;
    }
    return false;
}

bool DynaLoggerManager::removeDefaultSink(const char* fileNamePrefix) {
    auto* sink = getOutputSink(fileNamePrefix);
    if (sink != nullptr && _defSinkSet->contains(sink)) {
        _defSinkSet->deleteEntry(sink);
        return true;
    }
    return false;
}

DefaultSinkSet* DynaLoggerManager::getDefaultSinks() {
    return _defSinkSet;
}

void DynaLoggerManager::initAllLoggers() {
    if (_loggerMap->count() > 0) {
        for (DynaLogger* logger : _loggerMap->values()) {
            logger->addDefaultAppenders();
        }
    }
}

LogLevel DynaLoggerManager::getMinLogLevel() {
    return _minLogLevel;
}

LogLevel DynaLoggerManager::getMaxLogLevel() {
    return _maxLogLevel;
}
void DynaLoggerManager::setLogLevel(LogLevel level, bool resetOverride) {
    _minLogLevel = level == ALL ? NONE : level;
    _maxLogLevel = level;
    updateAllLogLevels(resetOverride);
}

void DynaLoggerManager::setMinLogLevel(LogLevel minLevel, bool resetOverride) {
    _minLogLevel = minLevel;
    _maxLogLevel = ALL;
    updateAllLogLevels(resetOverride);
}

void DynaLoggerManager::setMaxLogLevel(LogLevel maxLevel, bool resetOverride) {
    _minLogLevel = NONE;
    _maxLogLevel = maxLevel;
    updateAllLogLevels(resetOverride);
}

void DynaLoggerManager::setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool resetOverride) {
    _minLogLevel = minLevel;
    _maxLogLevel = maxLevel;
    updateAllLogLevels(resetOverride);
}

String& DynaLoggerManager::getRootLogDirectory() {
    return _rootLogDirectory;
}

void DynaLoggerManager::setLoggerLogLevels(const char* loggerName, LogLevel minLevel, LogLevel maxLevel, bool resetOverride) {
    DynaLogger* logger = _loggerMap->get(loggerName);
    if (logger != nullptr) {
        logger->setLogLevels(minLevel, maxLevel, resetOverride);
    }
}

void DynaLoggerManager::setOutputLogLevels(SinkType type, LogLevel minLevel, LogLevel maxLevel, bool resetOverride) {
    DynaOutputSink* sink = _outputTypeMap->get(type);
    if (sink != nullptr) {
        sink->setLogLevels(minLevel, maxLevel, resetOverride);
    }
}

void DynaLoggerManager::setOutputLogLevels(const char* filePrefixName, LogLevel minLevel, LogLevel maxLevel, bool resetOverride) {
    DynaOutputSink* sink = _outputFileMap->get(filePrefixName);
    if (sink != nullptr) {
        sink->setLogLevels(minLevel, maxLevel, resetOverride);
    }
}

void DynaLoggerManager::updateLoggerLogLevels(const char* name, bool resetOverride) {
    DynaLogger* logger = _loggerMap->get(name);
    if (logger != nullptr && (resetOverride || !logger->isLevelOverride())) {
        logger->setLogLevels(_minLogLevel, _maxLogLevel, resetOverride);
        logger->updateAllAppenderLogLevels(resetOverride);
    }
}

void DynaLoggerManager::updateAllLogLevels(bool resetOverride) {
    if (_loggerMap->count() > 0) {
        for (DynaLogger* logger : _loggerMap->values()) {
            if (resetOverride || !logger->isLevelOverride()) {
                logger->setLogLevels(_minLogLevel, _maxLogLevel, resetOverride);
                logger->updateAllAppenderLogLevels(resetOverride);
            }
        }
    }
    if (_outputTypeMap->count() > 0) {
        for (DynaOutputSink* sink : _outputTypeMap->values()) {
            if (resetOverride || !sink->isLevelOverride()) {
                sink->setLogLevels(_minLogLevel, _maxLogLevel, resetOverride);
            }
        }
    }
    if (_outputFileMap->count() > 0) {
        for (DynaOutputSink* sink : _outputFileMap->values()) {
            if (resetOverride || !sink->isLevelOverride()) {
                sink->setLogLevels(_minLogLevel, _maxLogLevel, resetOverride);
            }
        }
    }
}

//======================================================================================================
DynaOutputSink::DynaOutputSink() {    
}
DynaOutputSink::DynaOutputSink(SinkType type) :
     _type(type), _minLogLevel(NONE), _maxLogLevel(ALL), _levelOverride(false), _colors(NO_COLOR), _enabled(true) {
}
DynaOutputSink::~DynaOutputSink() {
//    delete _queue;
}

DynaOutputSink::DynaOutputSink(const DynaOutputSink& other) {
    _type = other._type;
}

DynaOutputSink* DynaOutputSink::copy() {
    throw MethodNotImplemented("Base-class version not callable");
}

int DynaOutputSink::hashCode() const {
    auto code = HashCoder();
    code.add(_type);
    return code.getCode();
}

bool DynaOutputSink::operator==(const DynaOutputSink& other) const {
    return false;
}

SinkType DynaOutputSink::getType() {
    return _type;
}

void DynaOutputSink::setManager(DynaLoggerManager* mgr) {
    _mgr = mgr;
}

void DynaOutputSink::setAnsiColors(ANSIColors colors) {
    _colors = colors;
}

void DynaOutputSink::setAutoFlush(bool autoFlush) {
    _autoFlush = autoFlush;
}

bool DynaOutputSink::isEnabled() {
    return _enabled;
}

void DynaOutputSink::setEnabled(bool enabled) {
    _enabled = enabled;
}

LogLevel DynaOutputSink::getMinLogLevel() {
    return _minLogLevel;
}

LogLevel DynaOutputSink::getMaxLogLevel() {
    return _maxLogLevel;
}

void DynaOutputSink::setLogLevel(LogLevel level) {
    _minLogLevel = level == ALL ? NONE : level;
    _maxLogLevel = level;
    _levelOverride = true;
}

void DynaOutputSink::setMinLogLevel(LogLevel minLevel) {
    _minLogLevel = minLevel;
    _maxLogLevel = ALL;
    _levelOverride = true;
}

void DynaOutputSink::setMaxLogLevel(LogLevel maxLevel) {
    _minLogLevel = NONE;
    _maxLogLevel = maxLevel;
    _levelOverride = true;
}

void DynaOutputSink::setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool override) {
    _minLogLevel   = minLevel;
    _maxLogLevel   = maxLevel;
    _levelOverride = override;
}

bool DynaOutputSink::isLevelOverride() {
    return _levelOverride;
}

bool DynaOutputSink::appendToQueue(LogLevel level, const char* outStr) {
    throw MethodNotImplemented("Base-class version not callable");
}

void DynaOutputSink::flush() {
    throw MethodNotImplemented("Base-class version not callable");
}

#define ANSI_OFF "\033[0m"

#define ANSI_BLACK_FG 30
#define ANSI_BLACK_BG 40
#define ANSI_RED_FG 31
#define ANSI_RED_BG 41
#define ANSI_GREEN_FG 32
#define ANSI_GREEN_BG 42
#define ANSI_YELLOW_FG 33
#define ANSI_YELLOW_BG 43
#define ANSI_BLUE_FG 34
#define ANSI_BLUE_BG 44
#define ANSI_MAGENTA_FG 35
#define ANSI_MAGENTA_BG 45
#define ANSI_CYAN_FG 36
#define ANSI_CYAN_BG 46
#define ANSI_WHITE_FG 37
#define ANSI_WHITE_BG 47

#define ANSI_BOLD_ON 1
#define ANSI_BOLD_OFF 21
#define ANSI_UNDERLINE_ON 4
#define ANSI_UNDERLINE_OFF 24
#define ANSI_INVERSE_ON 7
#define ANSI_INVERSE_OFF 27

#define ANSI1(x) sprintf(buf, "\033[%dm",x)
#define ANSI2(x1,x2) sprintf(buf, "\033[%d;%dm",x1,x2)
#define ANSI3(x1,x2,x3) sprintf(buf, "\033[%d;%d;%dm",x1,x2,x3)

String DynaOutputSink::ansiOn(LogLevel level) {
    string ansiStr;
    char buf[20];
    if (_colors == DARK_BG) {
        switch(level) {
            case FATAL:
                ANSI3( ANSI_BOLD_ON, ANSI_YELLOW_FG, ANSI_RED_BG );
                break;
            case CRITICAL:
                ANSI3( ANSI_BOLD_ON, ANSI_WHITE_FG, ANSI_MAGENTA_BG );
                break;
            case ERROR:
                ANSI2( ANSI_BOLD_ON, ANSI_RED_FG );
                break;
            case WARNING:
                ANSI2( ANSI_BOLD_ON, ANSI_YELLOW_FG );
                break;
            case INFO:
                ANSI1( 0 );
                break;
            case DEBUG:
                ANSI1( ANSI_CYAN_FG );
                break;
            case TRACE:
                ANSI1( ANSI_CYAN_FG );
                break;
            default:
                ANSI1( 0 );
                break;
        }
    }
    return String(buf);
}

//======================================================================================================
DynaOutputSinkConsole::DynaOutputSinkConsole() : DynaOutputSink(CONSOLE_SINK) {
//    _queue = new ste::concurrent_queue<String*>();
    setAnsiColors(DARK_BG);
    setLogLevels(INFO, TRACE);
}

DynaOutputSinkConsole::~DynaOutputSinkConsole() {
//    delete _queue;
}

DynaOutputSinkConsole::DynaOutputSinkConsole(const DynaOutputSinkConsole& other) : DynaOutputSink(other) {
}

DynaOutputSinkConsole* DynaOutputSinkConsole::copy() {
    return new DynaOutputSinkConsole(*this);
}

bool DynaOutputSinkConsole::appendToQueue(LogLevel level, const char* outStr) {
    if (_enabled && (level >= _minLogLevel && level <= _maxLogLevel)) {
        if (!_mgr->isDisableColours()) {
            cout << (ansiOn(level) + String(outStr) + String(ANSI_OFF)) << endl;
        }
        else {
            cout << (String(outStr)) << endl;
        }
        if (_autoFlush || _mgr->isAutoFlushAll()) {
            flush();
        }
        return true;
    }
    return false;
}

void DynaOutputSinkConsole::flush() {
    cout.flush();
}

//======================================================================================================
DynaOutputSinkError::DynaOutputSinkError() : DynaOutputSink(ERROR_SINK) {
//    _queue = new ste::concurrent_queue<String*>();
    setLogLevels(FATAL, WARNING);   // Sets these as overrides
    setAnsiColors(DARK_BG);
}

DynaOutputSinkError::~DynaOutputSinkError() {
//    delete _queue;
}

DynaOutputSinkError::DynaOutputSinkError(const DynaOutputSinkError& other) : DynaOutputSink(other) {
}

DynaOutputSinkError* DynaOutputSinkError::copy() {
    return new DynaOutputSinkError(*this);
}

bool DynaOutputSinkError::appendToQueue(LogLevel level, const char* outStr) {
    if (_enabled && (level >= _minLogLevel && level <= _maxLogLevel)) {
        if (!_mgr->isDisableColours()) {
            cerr << (ansiOn(level) + String(outStr) + String(ANSI_OFF)) << endl;
        }
        else {
            cerr << String(outStr) << endl;
        }
        return true;
    }
    return false;
}

void DynaOutputSinkError::flush() {
    // No need to flush because cerr has no buffering
}

//======================================================================================================

DynaOutputSinkFile::DynaOutputSinkFile(const char* fileNamePrefix) :
    DynaOutputSink(FILE_SINK),
    _fileNamePrefix(fileNamePrefix),
    _logFile(nullptr),
    _filePolicies(0),
    _maxFileSize(0),
    _maxFiles(0)
{
//    _queue = new ste::concurrent_queue<String*>();
    auto info = getCurrentTimeInfo();
    _curHourOfDay = info.tm_hour;
    _curDayOfYear = info.tm_yday;
}

DynaOutputSinkFile::~DynaOutputSinkFile() {
//    delete _queue;
    close();
}

DynaOutputSinkFile::DynaOutputSinkFile(const DynaOutputSinkFile& other) : DynaOutputSink(other) {
    _fileNamePrefix = other._fileNamePrefix;
    _fileName       = other._fileName;
    _logFile        = fdopen(dup(fileno(other._logFile)), "a");
}

DynaOutputSinkFile* DynaOutputSinkFile::copy() {
    return new DynaOutputSinkFile(*this);
}

int DynaOutputSinkFile::hashCode() const {
    auto code = HashCoder();
    code.add(_fileNamePrefix.c_str());
    if (!_fileName.empty()) {
        code.add(_fileName.c_str());
    }
    return code.getCode();
}

String& DynaOutputSinkFile::getFileNamePrefix() {
    return _fileNamePrefix;
}

String& DynaOutputSinkFile::getFileName() {
    return _fileName;
}

FILE* DynaOutputSinkFile::getFile() {
    return _logFile;
}

void DynaOutputSinkFile::setPolicies(uint16_t policyBits) {
    SET_BITS(_filePolicies, policyBits);
}

void DynaOutputSinkFile::clearPolicies(uint16_t policyBits) {
    CLEAR_BITS(_filePolicies, policyBits);
}

void DynaOutputSinkFile::setMaxFileSizeKB(uint32_t maxSize) {
    if (maxSize <= 0) {
        if (_maxFiles > 0) {
            DynaLoggerManager::getManagerLogger()->warning(
                "Setting a MaxFileSizeKB of 0 causes RollingMaxKeep to be 0 also...");
        }
        maxSize = 0;
        _maxFiles = 0;
    }
    else if (maxSize < MIN_MAX_FILE_SIZE_KB) {
        DynaLoggerManager::getManagerLogger()->warning(
            "Setting a MaxFileSizeKB below %d causes other values to be adjusted...", MIN_MAX_FILE_SIZE_KB);
        CLEAR_BITS(_filePolicies, ROLLING_ARCHIVE);
        maxSize = MIN_MAX_FILE_SIZE_KB;
        _maxFiles = 1;
    }
    _maxFileSize = maxSize * 1024;
}

uint64_t DynaOutputSinkFile::getMaxFileSizeKB() {
    return _maxFileSize;
}

void DynaOutputSinkFile::setRollingMaxKeep(int maxKeepFiles) {
    if (maxKeepFiles <= 0) {
        if (_maxFileSize > 0) {
            DynaLoggerManager::getManagerLogger()->warning(
                "Setting a RollingMaxKeep of 0 causes MaxFileSizeKB to be 0 also...");
        }
        maxKeepFiles = 0;
        _maxFileSize = 0;
    }
    else if (maxKeepFiles > MAX_ROLLING_KEEP) {
        DynaLoggerManager::getManagerLogger()->warning(
            "Setting a RollingMaxKeep greater than %d (yours is %d) causes "
            "the value to be limited at %d...", MAX_ROLLING_KEEP, maxKeepFiles, MAX_ROLLING_KEEP);
        maxKeepFiles = MAX_ROLLING_KEEP;
    }
    _maxFiles = maxKeepFiles;    
}

int DynaOutputSinkFile::getRollingMaxKeep() {
    return _maxFiles;
}

bool DynaOutputSinkFile::open() {
    close();
    _logFile = fopen(_fileName.c_str(), "a");
    return isOpen();
}

bool DynaOutputSinkFile::isOpen() {
    return _logFile != nullptr;
}

void DynaOutputSinkFile::close() {
    if (isOpen()) {
        fflush(_logFile);
        fclose(_logFile);
        _logFile = nullptr;
    }
}

tm DynaOutputSinkFile::getCurrentTimeInfo() {
    system_clock::time_point t1 = system_clock::now();
    time_t secs = duration_cast<seconds>(t1.time_since_epoch()).count();
    return *localtime(&secs);
}

bool DynaOutputSinkFile::fileNeedsToChange() {
    if (IS_ANY_SET(_filePolicies, TRIGGER_DATE_CHANGE | TRIGGER_HOUR_CHANGE)) {
        auto info = getCurrentTimeInfo();
        if (IS_ALL_SET(_filePolicies, TRIGGER_HOUR_CHANGE) && info.tm_hour != _curHourOfDay) {
            return true;
        }
        else if (info.tm_yday != _curDayOfYear) {
            return true;
        }
    }
    if (_maxFileSize > 0) {
        struct stat st;
        if (fstat(fileno(_logFile), &st) == 0) {
            if (st.st_size > _maxFileSize) {
                return true;
            }
        }
    }
    return false;
}

void DynaOutputSinkFile::shuffleFileNames(String& fileName, int index) {
    struct stat st;
    String newName = fileName + "." + to_string(index) + ".log";
    if (stat(newName.c_str(), &st) == 0) {
        if (_maxFiles <= 0 || index < _maxFiles) {
            shuffleFileNames(fileName, ++index);
        }
        else if (IS_ALL_SET(_filePolicies, ROLLING_ARCHIVE)) {
            auto  info = getCurrentTimeInfo();
            char  buf[200];
            strftime(buf, sizeof(buf), "_%H%M%S", &info);
            // Here is where the .gzip would be created...
            String gzName = newName + String(buf) + ".gz";
            rename(newName.c_str(), gzName.c_str());
        }
        else {
            unlink(newName.c_str());
        }
    }
    String prevName = fileName + (index > 1 ? ("." + to_string(index - 1)) + ".log" : ".log");
    rename(prevName.c_str(), newName.c_str());
}

void DynaOutputSinkFile::getNextFileName() {
    String tempStr = DynaLoggerManager::getSingleton()->getRootLogDirectory();
    if (tempStr.length() > 0) {
        tempStr += "/";
    }

    tempStr += _fileNamePrefix;

    // Here we append a date/time specifier if the policy requires it...
    if (IS_ANY_SET(_filePolicies, TRIGGER_DATE_CHANGE | TRIGGER_HOUR_CHANGE)) {
        auto  info = getCurrentTimeInfo();
        char  buf[200];
        char* format = (IS_ALL_SET(_filePolicies, TRIGGER_HOUR_CHANGE)) ? (char*)"_%Y-%m-%dH%H" : (char*)"_%Y-%m-%d";
        strftime(buf, sizeof(buf), format, &info);
        tempStr += buf;
    }

    // Here we get the next rolling index if the policy requires it...
    if (isOpen() && _maxFileSize > 0) {
        String tempStr2 = tempStr;
        struct stat st;
        if (fstat(fileno(_logFile), &st) == 0) {
            if (st.st_size > _maxFileSize) {
                // Open file needs to be switched out...
                close();
                shuffleFileNames(tempStr2, 1);
            }
        }
        tempStr += ".log";
    }
    else {
        tempStr += ".log";
    }
    _fileName = tempStr;
}

bool DynaOutputSinkFile::appendToQueue(LogLevel level, const char* outStr) {
    if (_enabled && (level >= _minLogLevel && level <= _maxLogLevel)) {
        if (!isOpen() || fileNeedsToChange()) {
            auto info = getCurrentTimeInfo();
            _curDayOfYear = info.tm_yday;
            _curHourOfDay = info.tm_hour;
            getNextFileName();
            open();
        }
        if (isOpen()) {
            string tmpStr       = string(outStr) + "\n";
            size_t bytesToWrite = tmpStr.length();
            size_t count        = fputs(tmpStr.c_str(), _logFile);
            if (count != bytesToWrite) {
                ;
            }
            if (_autoFlush || _mgr->isAutoFlushAll()) {
                fflush(_logFile);
            }
        }
        return true;
    }
    return false;
}

void DynaOutputSinkFile::flush() {
    if (isOpen()) {
        fflush(_logFile);
    }
}

//======================================================================================================

DynaLogger::DynaLogger() :
    _minLogLevel(NONE), _maxLogLevel(ALL), _traceCount(0), _traceCounter(0), _compactConsoleDebug(false) {
    _appenderTypeMap = new DynaAppenderTypeMap();
    _appenderFileMap = new DynaAppenderFileMap();
}

DynaLogger::~DynaLogger() {
    delete _appenderTypeMap;
    delete _appenderFileMap;
}

DynaLogger::DynaLogger(const DynaLogger& other) {
    _name            = other._name;
    _appenderTypeMap = other._appenderTypeMap->copy();
    _appenderFileMap = other._appenderFileMap->copy();
    _minLogLevel     = other._minLogLevel;
    _maxLogLevel     = other._maxLogLevel;
    _traceCount      = other._traceCount;
    _traceCounter    = 0;
}

DynaLogger* DynaLogger::copy() {
    return new DynaLogger(*this);
}

int DynaLogger::hashCode() const {
    auto code = HashCoder();
    code.add(_name.c_str());
    return code.getCode();
}

bool DynaLogger::operator==(const DynaLogger& other) const {
    return false;
}

DynaLogger* DynaLogger::getLogger(const char* name, bool addDefAppenders) {
    auto* manager = DynaLoggerManager::getSingleton();
    auto* logger  = manager->getLogger(name);
    if (logger == nullptr) {
        logger        = new DynaLogger();
        logger->_name = name;
        if (manager->registerLogger(name, logger)) {
            if (addDefAppenders) {
                logger->addDefaultAppenders();
            }
        }
    }
    return logger;
}

void DynaLogger::addDefaultAppenders() {
    auto* manager = DynaLoggerManager::getSingleton();
    for (DynaOutputSink* sink : manager->getDefaultSinks()->values()) {
        if (sink->getType() == FILE_SINK) {
            auto* fileSink = (DynaOutputSinkFile*)sink;
            auto  prefix   = fileSink->getFileNamePrefix();
            addAppender(prefix.c_str());
        }
        else {
            addAppender(sink->getType());
        }
    }
}

void DynaLogger::setTraceCount(long count) {
    _traceCount   = count;
    _traceCounter = 0;
}

long DynaLogger::getTraceCount() {
    return _traceCount;
}

void DynaLogger::resetTraceCounter() {
    _traceCounter = 0;
}

void DynaLogger::setCompactConsoleDebug(bool compactConsole) {
    _compactConsoleDebug = compactConsole;
}

DynaAppender* DynaLogger::addAppender(DynaAppender* appender) {
    bool added = false;
    if (appender->getType() == FILE_SINK) {
        auto* fileSink = (DynaOutputSinkFile*)(appender->getOutputSink());
        auto  prefix   = fileSink->getFileNamePrefix();
        if (!_appenderFileMap->containsKey(&prefix)) {
            _appenderFileMap->put(new String(prefix.c_str()), appender);
            added = true;
        }
    }
    else {
        auto sinkType = ESinkType(appender->getType());
        if (!_appenderTypeMap->containsKey(&sinkType)) {
            _appenderTypeMap->put(new ESinkType(sinkType), appender);
            added = true;
        }
    }
    // Default the appender's levels to match the current logger levels...
    if (added) {
        appender->setLogLevels(_minLogLevel, _maxLogLevel, false);
    }
    return appender;
}

DynaAppender* DynaLogger::addAppender(SinkType type) {
    auto* appender = getAppender(type);
    if (appender == nullptr) {
        appender = DynaAppender::getTypedAppender(type);
        addAppender(appender);
    }
    return appender;
}

DynaAppender* DynaLogger::addAppender(const char* fileNamePrefix) {
    auto* appender = getAppender(fileNamePrefix);
    if (appender == nullptr) {
        appender = DynaAppender::getFileAppender(fileNamePrefix);
        addAppender(appender);
    }
    return appender;
}

DynaAppender* DynaLogger::getAppender(SinkType type) {
    return _appenderTypeMap->get(type);
}

DynaAppender* DynaLogger::getAppender(const char* fileNamePrefix) {
    return _appenderFileMap->get(fileNamePrefix);
}

bool DynaLogger::removeAppender(SinkType type) {
    auto key = ESinkType(type);
    if (_appenderTypeMap->containsKey(&key)) {
        _appenderTypeMap->deleteEntry(&key);
        return true;
    }
    return false;
}

bool DynaLogger::removeAppender(const char* fileNamePrefix) {
    auto key = String(fileNamePrefix);
    if (_appenderFileMap->containsKey(&key)) {
        _appenderFileMap->deleteEntry(&key);
        return true;
    }
    return false;
}

LogLevel DynaLogger::getMinLogLevel() {
    return _minLogLevel;
}

LogLevel DynaLogger::getMaxLogLevel() {
    return _maxLogLevel;
}

void DynaLogger::setLogLevel(LogLevel level) {
    _minLogLevel = level == ALL ? NONE : level;
    _maxLogLevel = level;
    _levelOverride = true;
    updateAllAppenderLogLevels();
}

void DynaLogger::setMinLogLevel(LogLevel minLevel) {
    _minLogLevel = minLevel;
    _maxLogLevel = ALL;
    _levelOverride = true;
    updateAllAppenderLogLevels();
}

void DynaLogger::setMaxLogLevel(LogLevel maxLevel) {
    _minLogLevel = NONE;
    _maxLogLevel = maxLevel;
    _levelOverride = true;
    updateAllAppenderLogLevels();
}

void DynaLogger::setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool override) {
    _minLogLevel   = minLevel;
    _maxLogLevel   = maxLevel;
    _levelOverride = override;
    updateAllAppenderLogLevels();
}

void DynaLogger::setAppenderLogLevels(SinkType type, LogLevel minLevel, LogLevel maxLevel) {
    auto* appender = _appenderTypeMap->get(type);
    if (appender != nullptr) {
        appender->setLogLevels(minLevel, maxLevel);
    }
}

void DynaLogger::setAppenderLogLevels(const char* filePrefixName, LogLevel minLevel, LogLevel maxLevel) {
    auto* appender = _appenderFileMap->get(filePrefixName);
    if (appender != nullptr) {
        appender->setLogLevels(minLevel, maxLevel);
    }
}

bool DynaLogger::isLevelOverride() {
    return _levelOverride;
}

void DynaLogger::updateAppenderLogLevels(SinkType type, bool resetOverride) {
    auto* appender = _appenderTypeMap->get(type);
    if (appender != nullptr && (resetOverride || !appender->isLevelOverride())) {
        appender->setLogLevels(_minLogLevel, _maxLogLevel, false);
    }
}

void DynaLogger::updateAppenderLogLevels(const char* fileNamePrefix, bool resetOverride) {
    auto* appender = _appenderFileMap->get(fileNamePrefix);
    if (appender != nullptr && (resetOverride || !appender->isLevelOverride())) {
        appender->setLogLevels(_minLogLevel, _maxLogLevel, false);
    }
}

void DynaLogger::updateAllAppenderLogLevels(bool resetOverride) {
    if (_appenderFileMap->count() > 0) {
        for (DynaAppender* appender : _appenderFileMap->values()) {
            if (resetOverride || !appender->isLevelOverride()) {
                appender->setLogLevels(_minLogLevel, _maxLogLevel, false);
            }
        }
    }
    if (_appenderTypeMap->count() > 0) {
        for (DynaAppender* appender : _appenderTypeMap->values()) {
            if (resetOverride || !appender->isLevelOverride()) {
                appender->setLogLevels(_minLogLevel, _maxLogLevel, false);
            }
        }
    }
}

system_clock::time_point DynaLogger::_getNow() {
    return system_clock::now();
}

String DynaLogger::_getDateTime(system_clock::time_point t1, const char* format) {
    long   milli  = TimeUtils::getEpochMillis(t1);
    return String(TimeUtils::getLocalTime(milli, format));
}

void DynaLogger::_log(LogLevel level, const char* msg, va_list args) {
    if (level >= _minLogLevel && level <= _maxLogLevel) {
        char buf1[MAX_LOG_ENTRY_SIZE];
        char buf2[MAX_LOG_ENTRY_SIZE];
        vsnprintf(buf1, MAX_LOG_ENTRY_SIZE, msg, args);
        snprintf(buf2, MAX_LOG_ENTRY_SIZE, "[%-8s] %s", logLevelName[level], buf1);
        auto* buf = buf2;
        system_clock::time_point t1 = _getNow();
        auto  timestamp = _getDateTime(t1, "%Y-%m-%d %T");

        if (_appenderTypeMap->count() > 0) {
            for (DynaAppender* appender : _appenderTypeMap->values()) {
                if (_compactConsoleDebug && (appender->getType() == CONSOLE_SINK)) {
                    auto  shortTime = _getDateTime(t1, "%T");
                    snprintf(buf1, MAX_LOG_ENTRY_SIZE, "[%s] [%-10s] %s", shortTime.c_str(), _name.c_str(), buf);
                }
                else {
                    snprintf(buf1, MAX_LOG_ENTRY_SIZE, "[%s] [%-10s] [%-7s] %s", timestamp.c_str(),
                        _name.c_str(), sinkTypeName[appender->getType()], buf);
                }
                appender->append(level, buf1);
            }
        }
        if (_appenderFileMap->count() > 0) {
            snprintf(buf1, MAX_LOG_ENTRY_SIZE, "[%s] [%-10s] %s", timestamp.c_str(), _name.c_str(), buf);
            for (DynaAppender* appender : _appenderFileMap->values()) {
                appender->append(level, buf1);
            }
        }
    }
}

void DynaLogger::log(LogLevel level, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(level, msg, args);
    va_end(args);
}

void DynaLogger::fatal(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(FATAL, msg, args);
    va_end(args);
}

void DynaLogger::critical(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(CRITICAL, msg, args);
    va_end(args);
}

void DynaLogger::error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(ERROR, msg, args);
    va_end(args);
}

void DynaLogger::warning(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(WARNING, msg, args);
    va_end(args);
}

void DynaLogger::info(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(INFO, msg, args);
    va_end(args);
}

void DynaLogger::debug(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    _log(DEBUG, msg, args);
    va_end(args);
}

void DynaLogger::trace(const char* msg, ...) {
    if (_traceCount <= 0 || ++_traceCounter >= _traceCount) {
        va_list args;
        va_start(args, msg);
        _log(TRACE, msg, args);
        va_end(args);
        _traceCounter = 0;
    }
}

//======================================================================================================
DynaAppender::DynaAppender(SinkType type) : _type(type), _minLogLevel(NONE), _maxLogLevel(ALL) {
}

DynaAppender::~DynaAppender() {
}

DynaAppender::DynaAppender(const DynaAppender& other) {
    _type        = other._type;
    _output      = other._output;
    _minLogLevel = other._minLogLevel;
    _maxLogLevel = other._maxLogLevel;
}

DynaAppender* DynaAppender::copy() {
    return new DynaAppender(*this);
}

int DynaAppender::hashCode() const {
    auto code = HashCoder();
    code.add(_type);
    if (_output != nullptr) {
        code.add(_output->hashCode());
    }
    return code.getCode();
}

bool DynaAppender::operator==(const DynaAppender& other) const {
    return false;
}

DynaAppender* DynaAppender::getTypedAppender(SinkType sinkType) {
    auto* appender   = new DynaAppender(sinkType);
    auto* outputSink = DynaLoggerManager::getSingleton()->addOutputSink(sinkType);
    appender->setOutputSink(outputSink);
    return appender;
}

DynaAppender* DynaAppender::getConsoleAppender() {
    return getTypedAppender(CONSOLE_SINK);
}

DynaAppender* DynaAppender::getErrorAppender() {
    return getTypedAppender(ERROR_SINK);
}

DynaAppender* DynaAppender::getFileAppender(const char* fileNamePrefix) {
    auto* appender   = new DynaAppender(FILE_SINK);
    auto* outputSink = DynaLoggerManager::getSingleton()->addOutputSink(fileNamePrefix);
    appender->setOutputSink(outputSink);
    return appender;
}

LogLevel DynaAppender::getMinLogLevel() {
    return _minLogLevel;
}

LogLevel DynaAppender::getMaxLogLevel() {
    return _maxLogLevel;
}

void DynaAppender::setLogLevel(LogLevel level) {
    _minLogLevel = level == ALL ? NONE : level;
    _maxLogLevel = level;
    _levelOverride = true;
}

void DynaAppender::setMinLogLevel(LogLevel minLevel) {
    _minLogLevel = minLevel;
    _maxLogLevel = ALL;
    _levelOverride = true;
}

void DynaAppender::setMaxLogLevel(LogLevel maxLevel) {
    _minLogLevel = NONE;
    _maxLogLevel = maxLevel;
    _levelOverride = true;
}

void DynaAppender::setLogLevels(LogLevel minLevel, LogLevel maxLevel, bool override) {
    _minLogLevel   = minLevel;
    _maxLogLevel   = maxLevel;
    _levelOverride = override;
}

bool DynaAppender::isLevelOverride() {
    return _levelOverride;
}

DynaOutputSink* DynaAppender::getOutputSink() {
    return _output;
}

void DynaAppender::setOutputSink(DynaOutputSink* sink) {
    _output = sink;
}

SinkType DynaAppender::getType() {
    return _type;
}

bool DynaAppender::append(LogLevel level, const char* outStr) {
    if (level >= _minLogLevel && level <= _maxLogLevel) {
        _output->appendToQueue(level, outStr);
        return true;
    }
    return false;
}
