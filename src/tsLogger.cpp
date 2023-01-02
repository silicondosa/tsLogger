#include <tsLogger.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <type_traits>
#include <vector>
#include <algorithm>

// Template to handle undefined visitor types of variants
template<class> inline constexpr bool always_false_v = false;

int tsLogger::initLogger()
{
	// Allocate buffer memory dynamically
	logBuffer		= (char*)	malloc(logBufferSize);
	preamble.clear();
	tsHeader.clear();
	dataVector.clear();

	// Open data file
	if (fileType == _TSLOGGER_CSV) {
		fopen_s(&(this->logFilePtr), fileName, "w+");
		setvbuf(logFilePtr, logBuffer, _IOLBF, logBufferSize);
	}
	else {
		fopen_s(&(this->logFilePtr), fileName, "wb+");
	}

	// Set internal parameters of the logger object
	numHeaderLines	= 0;
	dataVecLen		= dataVector.size();

	canAddPreamble		= true;
	canAddDataHeader	= true;
	isLogggingStart		= false;

	return 0;
}

tsLogger::tsLogger()
{
	logUnixTime		= time(NULL);
	tm localTime;
	localtime_s(&localTime, &logUnixTime);

	snprintf(fileName, fileNameLength, "%s\\%s_%04d-%02d-%02d-%02d%02d%02d.csv",
										defaultFilePath,			/*File directory*/
										defaultFileName,			/*File name prefix*/
										localTime.tm_year+1900,		/*Current year*/
										localTime.tm_mon+1,			/*Current Month*/
										localTime.tm_mday,			/*Current Date*/
										localTime.tm_hour,			/*Current Hour of day   (24-hour mode)*/
										localTime.tm_min,			/*Current Minute of day (24-hour mode)*/
										localTime.tm_sec);			/*Current Seconds*/
	fileType		= _TSLOGGER_CSV;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(tsLogFileType logFileType)
{
	logUnixTime		= time(NULL);
	tm localTime;
	localtime_s(&localTime, &logUnixTime);

	snprintf(fileName, fileNameLength, "%s\\%s_%04d-%02d-%02d-%02d%02d%02d.csv",
										defaultFilePath,			/*File directory*/
										defaultFileName,			/*File name prefix*/
										localTime.tm_year+1990,		/*Current year*/
										localTime.tm_mon+1,			/*Current Month*/
										localTime.tm_mday,			/*Current Date*/
										localTime.tm_hour,			/*Current Hour of day   (24-hour mode)*/
										localTime.tm_min,			/*Current Minute of day (24-hour mode)*/
										localTime.tm_sec);			/*Current Seconds*/
	fileType		= logFileType;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(const char* logFileName)
{
	logUnixTime		= time(NULL);
	snprintf(fileName, fileNameLength, "%s", logFileName);
	fileType		= _TSLOGGER_CSV;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(const char* logFileName, tsLogFileType logFileType)
{
	logUnixTime		= time(NULL);
	snprintf(fileName, fileNameLength, "%s", logFileName);
	fileType		= logFileType;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(const char* logFileName, tsLogFileType logFileType, size_t logBufferMaxSize)
{
	logUnixTime		= time(NULL);
	snprintf(fileName, fileNameLength, "%s", logFileName);
	fileType		= logFileType;
	logBufferSize	= logBufferMaxSize;

	initLogger();
}

void tsLogger::getFilePath(char* pathDestination, size_t pathDestinationLength)
{
	strcpy_s(pathDestination, pathDestinationLength, this->fileName);
}

size_t tsLogger::getNumDataPoints()
{
	return dataVecLen;
}

void tsLogger::appendToPreamble(std::string appendString)
{
	std::replace(appendString.begin(), appendString.end(), '\n', ' '); // replace all new line characters with spaces.
	this->preamble.append(appendString);
	this->preamble.push_back('\n');
	this->numHeaderLines++;
}

void tsLogger::addPreamble()
{
	// Can add preamble only if this is the first time you are adding it,
	// and neither the dataHeader has been input nor the logging started.
	if (canAddPreamble == true && this->canAddDataHeader == true && isLogggingStart == false) {
		this->numHeaderLines++;
		fprintf(this->logFilePtr, "%05d,%lld\n%s", this->numHeaderLines, (long long)this->logUnixTime, this->preamble.c_str());
		canAddPreamble = false;
	}
}

void tsLogger::addDataHeader()
{
	if (this->canAddDataHeader == true && isLogggingStart == false) {
		if (this->canAddPreamble == false) {
			fseek(this->logFilePtr, 0, SEEK_SET);
			this->numHeaderLines++;
			fprintf(this->logFilePtr, "%05d", this->numHeaderLines);
			fseek(this->logFilePtr, 0, SEEK_END);
		}
		for (auto it = std::begin(this->dataVector); it != std::end(this->dataVector); /*increment is in loop*/) {
			if (this->fileType == _TSLOGGER_CSV) {
				fprintf(this->logFilePtr, "%s", it->dataName.c_str());
				if (++it != this->dataVector.end()) {
					fprintf(this->logFilePtr, ",");
				}
				else {
					fprintf(this->logFilePtr, "\n");
				}
			}
		}
		this->canAddDataHeader = false;
	}
}

void tsLogger::startLogger()
{
	if (isLogggingStart == false) {
		isLogggingStart = true;
	}
}

void tsLogger::logData()
{
	smartDataPtr dataPtr;
	if (isLogggingStart != false) {
		this->startLogger();
	}
	for (auto it = std::begin(this->dataVector); it != std::end(this->dataVector); /*increment is in loop*/) {
		if (this->fileType == _TSLOGGER_CSV) {
			
			// Handle variant types here
			//--------------------------
			dataPtr = std::visit([](auto&& arg) -> smartDataPtr {return arg;}, it->dataPtr);
			std::visit([this](auto&& arg) {
				using dataPtrType = std::decay_t<decltype(arg)>;
				
			// data pointer is int*
				if constexpr (std::is_same_v<dataPtrType, int*>)
					fprintf(this->logFilePtr, "%d", *arg);
			// data pointer is unsigned int*
				else if constexpr (std::is_same_v<dataPtrType, unsigned int*>)
					fprintf(this->logFilePtr, "%u", *arg);
			// data pointer is long*
				else if constexpr (std::is_same_v<dataPtrType, long*>)
					fprintf(this->logFilePtr, "%ld", *arg);
			// data pointer is unsigned long*
				else if constexpr (std::is_same_v<dataPtrType, unsigned long*>)
					fprintf(this->logFilePtr, "%lu", *arg);
			// data pointer is long long*
				else if constexpr (std::is_same_v<dataPtrType, long long*>)
					fprintf(this->logFilePtr, "%lld", *arg);
			// data pointer is unsigned long long*
				else if constexpr (std::is_same_v<dataPtrType, unsigned long long*>)
					fprintf(this->logFilePtr, "%llu", *arg);
			// data pointer is float*
				else if constexpr (std::is_same_v<dataPtrType, float*>)
					fprintf(this->logFilePtr, "%f", *arg);
			// data pointer is double*
				else if constexpr (std::is_same_v<dataPtrType, double*>)
					fprintf(this->logFilePtr, "%f", *arg);
			// data pointer is char*
				else if constexpr (std::is_same_v<dataPtrType, char*>)
					fprintf(this->logFilePtr, "%c", *arg);
			// data pointer is unsigned char*
				else if constexpr (std::is_same_v<dataPtrType, unsigned char*>)
					fprintf(this->logFilePtr, "%hhu", *arg);
			// data pointer is bool*
				else if constexpr (std::is_same_v<dataPtrType, bool*>)
					fprintf(this->logFilePtr, "%d", *arg);
			// handle unsupported variant visitor types of data pointer
				else
					static_assert(always_false_v<dataPtrType>, "tsLogger: FATAL ERROR: attempting to log unsupported data type!");
				}, dataPtr);

			if (++it != this->dataVector.end()) {
					fprintf(this->logFilePtr, ",");
			}
			else {
				fprintf(this->logFilePtr, "\n");
			}
		}
	}
}

tsLogger::~tsLogger()
{
	// Flush buffer and close file
	fflush(logFilePtr);
	fclose(logFilePtr);
	
	// Free all dynamic memory allocated to logger object
	free(logBuffer);
	dataVector.clear();

	numHeaderLines	= 0;
	dataVecLen		= 0;
	isLogggingStart = false;
}
