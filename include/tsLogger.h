#pragma once
#ifndef _TSLOGGER_H
#define _TSLOGGER_H
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <algorithm>
#include <ctime>

#define defaultFilePath		"C:\\data"
#define defaultFileName		"tsDataLog"
#define defaultDataName		"datapoint"
#define fileNameLength		400
#define defLogBufferSize	1000
#define defHeaderBufSize	1000
#define defDataBufSize		100

/**
 * @brief The file-types supported by tsLogger. [BINARY support coming in the future]
*/
typedef enum _tsLogFileType
{
	_TSLOGGER_CSV			= 0,
	_TSLOGGER_BINARY		= 1
}tsLogFileType;

/**
 * @brief This varriant datatype governs the supported data points that can be tracked.
*/
using smartDataPtr = std::variant	<int*, unsigned int*, \
									long*, unsigned long*, \
									long long*,	unsigned long long*, \
									float*, double*, \
									char*, unsigned char*, \
									bool* >;

/**
 * @brief Data pointer structure used to track datapoints, with datapoint name and pointer to data.
*/
struct tsDataPointer {
	std::string	dataName; // data name
	smartDataPtr dataPtr; // variant pointer to data	
};

/**
 * @brief The core class of the library is the tsLogger class. Multiple simultaneous class objects are supported.
*/
class tsLogger {
	// File properties
	time_t				logUnixTime;
	char				fileName[fileNameLength];
	tsLogFileType		fileType;
	FILE*				logFilePtr;


	// Buffers
	size_t						logBufferSize;
	char*						logBuffer;
	std::string					preamble; // stores info like number of preamble+header lines, time of file creation
	std::string					tsHeader;
	std::vector<tsDataPointer>	dataVector;

	// Logger object parameters
	unsigned			numHeaderLines;
	size_t				dataVecLen;
	bool				canAddPreamble, canAddDataHeader, isLogggingStart;

	// Private member functions

	/**
	 * @brief A private member-function of \ref tsLogger used by the constructors to initialize the class object.
	 * @return Returns 0 if successful and a negative error code if there was an error.
	*/
	int initLogger();

	// Public member functions
public:
	// tsLogger Constructors

	/**
	 * @brief Default constructor of the \ref tsLogger class.
	 * Uses default values to initialize all object properties.
	*/
	tsLogger();

	/**
	 * @brief An overloaded 1-parameter constructor of the \ref tsLogger class. 
	 * Allows the user to set the logfile type.
	 * @param logFileType The file type of the logfile per the \ref tsLogFileType enum.
	*/
	tsLogger(tsLogFileType logFileType);

	/**
	 * @brief An overloaded 1-parameter constructor of the \ref tsLogger class.
	 * Allows the user to set the logfile name.
	 * @param logFileName The file name of the logfile. You can optionally provide the full file path if you want the file to reside in a particular location.
	*/
	tsLogger(const char* logFileName);

	/**
	 * @brief An overloaded 2-parameter constructor of the \ref tsLogger class.
	 * Allows the user to set the file type and file name.
	 * @param logFileName The file name of the logfile. You can optionally provide the full file path if you want the file to reside in a particular location.
	 * @param logFileType The file type of the logfile per the \ref tsLogFileType enum.
	*/
	tsLogger(const char* logFileName, tsLogFileType logFileType);

	/**
	 * @brief An overloaded 3-parameter constructor of the \ref tsLogger class.
	 * Allows the user to select file type, file name and internal log buffer size.
	 * @param logFileName The file name of the logfile. You can optionally provide the full file path if you want the file to reside in a particular location.
	 * @param logFileType The file type of the logfile per the \ref tsLogFileType enum.
	 * @param logBufferMaxSize The buffer size of the internal log buffer. When the buffer is full, the data is dumped to file.
	*/
	tsLogger(const char* logFileName, tsLogFileType logFileType, size_t logBufferMaxSize);

	// Functions to read logger state

	/**
	 * @brief Allows the user to read the file path of the log file.
	 * @param pathDestination The pointer to the character array to which the user wants the file path to be written.
	 * @param pathDestinationLength The maximum length of data allowed to be written to the character array.
	*/
	void getFilePath(char* pathDestination, size_t pathDestinationLength);

	/**
	 * @brief Returns the total number of data points being tracked for logging by the \ref tsLogger class object.
	 * @return The number of data points being tracked by the logger object.
	*/
	size_t getNumDataPoints();

	// Functions/templates to modify logger state

	/**
	 * @brief Template member function that appends the data point to the list of tracked data.
	 * @tparam T Template data type of the data point that needs to be tracked. Support data types are governed by \ref smartDataPtr.
	 * @param dataPointer Pointer/reference to the data point that needs to be tracked.
	 * @param dataName Name of the data point that needs to be tracked. This name is used to build the header line of the log file.
	 * @return The total number of data points being tracked including the new data point if successful and a negative number if there was an error.
	*/
	template <typename T> unsigned trackDataPoint(T* dataPointer, const std::string& dataName)
	{
		tsDataPointer newData;
		if (canAddDataHeader != true) {
			return -1; // data cannot be added after the data header has been written to file.
		}
		if (isLogggingStart != false) {
			return -2; // data can't be added after logging has started.
		}
		if (dataPointer != nullptr) {
			if (dataName.empty()) {
				newData.dataName = defaultDataName + std::to_string(this->dataVecLen);
			}
			else {
				newData.dataName = dataName;
				// sanity check name so there is no trouble when eventually inserting names in the CSV header
				std::replace(newData.dataName.begin(), newData.dataName.end(), '\n', ' '); // replace all new line characters with spaces
				std::replace(newData.dataName.begin(), newData.dataName.end(), ',' , ' '); // replace all comma characters with spaces
				std::replace(newData.dataName.begin(), newData.dataName.end(), '\r', ' '); // replace all carriage return characters with spaces
				std::replace(newData.dataName.begin(), newData.dataName.end(), '\0', ' '); // replace all EOF characters with spaces
			}
			newData.dataPtr = dataPointer;

			this->dataVector.push_back(newData);
			this->dataVecLen = this->dataVector.size();
		}
		return this->dataVecLen;
	}

	/**
	 * @brief Appends a line of text to the preamble block of the log file.
	 * Replaces new line characters with spaces.
	 * @param appendString String to append.
	*/
	void appendToPreamble(std::string appendString);

	/**
	 * @brief Adds the preamble to the log file-can only be added once and only be called before the \ref addDataHeader function.
	*/
	void addPreamble();

	/**
	 * @brief Add the data header (consisting of datapoint names separated by commas) to the header file. 
	 * Do not call \ref trackDataPoint after this function call.
	*/
	void addDataHeader();
	
	// Functions to log data

	/**
	 * @brief Starts the logger and locks out the ability to add preambles, headers and new data points.
	*/
	void startLogger();

	/**
	 * @brief Logs the data points being tracked as a line in the log file.
	*/
	void logData();

	// tsLogger Destructor
	~tsLogger();
};

#endif // !_TSLOGGER_H

