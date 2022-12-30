#pragma once
#ifndef _TSLOGGER_H
#define _TSLOGGER_H
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <algorithm>

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

struct tsDataPointer {
	std::string	dataName;
	// variant data pointer
	std::variant	<int*,	unsigned int*, \
					long*,	unsigned long*, \
					float*, double*, \
					char*,	unsigned char*, \
					bool* \
					> dataPtr;
};

class tsLogger {
	// File properties
	char				fileName[fileNameLength];
	tsLogFileType		fileType;
	FILE*				logFilePtr;

	bool				canAddPreamble, canAddDataHeader;

	// Buffers
	size_t						logBufferSize;
	char*						logBuffer;
	std::string					preamble; // stores info like number of preamble+header lines, time of file creation
	std::string					tsHeader;
	std::vector<tsDataPointer>	dataVector;


	// Logger object parameters
	unsigned			numHeaderLines;
	size_t				dataVecLen;

	bool				isLogggingStart;

	// Private member functions
	int initLogger();

	// Public member functions
public:
	// tsLogger Constructors
	tsLogger();
	tsLogger(tsLogFileType logFileType);
	tsLogger(const char* logFileName);
	tsLogger(const char* logFileName, tsLogFileType logFileType);
	tsLogger(const char* logFileName, tsLogFileType logFileType, size_t logBufferMaxSize);

	// Functions to read logger state
	void getFilePath(char* pathDestination, size_t pathDestinationLength);
	size_t getNumDataPoints();

	// Functions/templates to modify logger state
	template <typename T> unsigned linkDataPoint(T* dataPointer, const std::string& dataName)
	{
		tsDataPointer newData;
		if (dataName.empty()) {
			newData.dataName = "Datapoint" + std::to_string(this->dataVecLen);
		}
		else {
			newData.dataName = dataName;
			std::replace(newData.dataName.begin(), newData.dataName.end(), '\n', ' '); // replace all new line characters with spaces
		}
		newData.dataPtr		= dataPointer;
		
		this->dataVector.push_back(newData);
		this->dataVecLen	= this->dataVector.size();

		return this->dataVecLen;
	}

	void appendToPreamble(std::string appendString);
	void addPreamble();
	void addDataHeader();
	
	// Functions to log data
	void startLogger();
	void logTimeSeriesData();

	// tsLogger Destructor
	~tsLogger();
};

#endif // !_TSLOGGER_H

