#include <tsLogger.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

int tsLogger::initLogger()
{
	// Allocate buffer memory dynamically
	logBuffer		= (char*)	malloc(logBufferSize);
	preHeader		= (char*)	malloc(defHeaderBufSize);
	csvHeader		= (char*)	malloc(defHeaderBufSize);
	dataBuffer		= (double*)	malloc(defDataBufSize * sizeof(double));

	// Open data file
	if (fileType == _TSLOGGER_CSV) {
		logFilePtr = fopen(fileName, "w+");
		setvbuf(logFilePtr, logBuffer, _IOLBF, logBufferSize);
	}
	else {
		logFilePtr = fopen(fileName, "wb+");
	}

	// Set internal parameters of the logger object
	numHeaderLines	= 1;
	numDataPoints	= 0;

	return 0;
}

tsLogger::tsLogger()
{
	time_t unixTime = time(NULL);
	tm* localTime = localtime(&unixTime);

	snprintf(fileName, fileNameLength, "%s\\%s_%04d-%02d-%02d-%02d%02d-%02d.csv",
										defaultFilePath,			/*File directory*/
										defaultFileName,			/*File name prefix*/
										localTime->tm_year+1990,	/*Current year*/
										localTime->tm_mon+1,		/*Current Month*/
										localTime->tm_mday,			/*Current Date*/
										localTime->tm_hour,			/*Current Hour of day   (24-hour mode)*/
										localTime->tm_min,			/*Current Minute of day (24-hour mode)*/
										localTime->tm_sec);			/*Current Seconds*/
	fileType		= _TSLOGGER_CSV;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(char* logFileName)
{
	snprintf(fileName, fileNameLength, "%s", logFileName);
	fileType		= _TSLOGGER_CSV;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(char* logFileName, tsLogFileType logFileType)
{
	snprintf(fileName, fileNameLength, "%s", logFileName);
	fileType		= logFileType;
	logBufferSize	= defLogBufferSize;

	initLogger();
}

tsLogger::tsLogger(char* logFileName, tsLogFileType logFileType, size_t logBufferMaxSize)
{
	snprintf(fileName, fileNameLength, "%s", logFileName);
	fileType		= logFileType;
	logBufferSize	= logBufferMaxSize;

	initLogger();
}

tsLogger::~tsLogger()
{
	// Flush buffer and close file
	fflush(logFilePtr);
	fclose(logFilePtr);
	
	// Free all dynamic memory allocated to logger object
	free(logBuffer);
	free(preHeader);
	free(csvHeader);
	free(dataBuffer);
	
}
