#pragma once
#ifndef _TSLOGGER_H
#define _TSLOGGER_H
#include <cstdio>

#define defaultFilePath		"C:\\data"
#define defaultFileName		"timeseriesData"
#define fileNameLength		400
#define defLogBufferSize	1000
#define defHeaderBufSize	1000
#define defDataBufSize		100

typedef enum _tsLogFileType
{
	_TSLOGGER_CSV			= 0,
	_TSLOGGER_BINARY		= 1
}tsLogFileType;

class tsLogger {
	// File properties
	char				fileName[fileNameLength];
	tsLogFileType		fileType;
	FILE*				logFilePtr;

	// Buffers
	size_t				logBufferSize;
	char*				logBuffer;
	char*				preHeader;
	char*				csvHeader;
	double*				dataBuffer;

	// Logger object parameters
	unsigned			numHeaderLines;
	unsigned			numDataPoints;
	
	// Private member functions
	int initLogger();

	// Public member functions
public:
	tsLogger();
	tsLogger(char* logFileName);
	tsLogger(char* logFileName, tsLogFileType logFileType);
	tsLogger(char* logFileName, tsLogFileType logFileType, size_t logBufferMaxSize);

	void addHeaderLine(char* headerLine);
	void addLogDataPoint(double* dataPoint);

	void logTimeSeriesData();

	~tsLogger();
};

#endif // !_TSLOGGER_H

