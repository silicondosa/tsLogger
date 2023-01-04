# tsLogger: Timeseries and Snapshot Logger
tsLogger (short for **t**imeseries and **s**napshot **Logger**) is a C++17 data logger for time series data that is dead-simple to use. It can also be used to save a snapshot the state of most variables to a file at any time during runtime.

## Features, requirements and limitations
- This is a C++17 library which utilizes `std::variant` vectors of pointers to data that needs to be tracked. So, all you need to log a variable its address. But, you will need a C++17 compatible compiler. The library has been tested to work on Visual Studio 2022 running on Windows 10.
- You can get started with logging in as few as one class object initialization followed by two function calls.
- Supports logging `int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, char, unsigned char, and bool` datatypes as a CSV file (I am working on binary file support).
- Users can optionally name the variables they are tracking too. Otherwise, a default name (variable number being tracked: _0~n-1_) is assigned.
- The user can optionally add either/both a preamble or/and a CSV header to the log file.
    - The CSV header contains the aforementioned variable names.
    - The first line of the preamble contains the total number of lines the preamble and header take together in the text file along with the UNIX time stamp when the library was initialized---separated by a comma.
    - Users can append lines of text to the preamble as they see it---I use it to store calibration data pertinent to the experiment that I am running.
    - The preamble must be added first, before the header and both need to be added to the file before logging begins to allows for easy ingestion of CSV data to post-processing software like Python or MATLAB. This is also why the very first line contains the number of lines to ignore before reading CSV data.
- This library is not thread-safe. This is something I want to investigate, but don't know if my architecture will allow it.

## Prerequisites, usage and DOCUMENTATION
The only prerequisite to use this library is a C++17 compatible compiler. I've tested it to work on Windows, but it may need some tweaks to get it up and running on Linux/Mac.

The header file `tsLogger.h` is located in the `./include` directory. You can either use the precompiled _win32_ static library (Win32) in `./bin` or compile it yourself using the only source file located at `./src/tsLogger.cpp` if you're on a different platform or even if you just want to.

I will make DOXYGEN-generated documentation available as soon as I have a minute to comment the files and build the documentation. In the mean time, here is a list of the functions in the order that I would use them in my code:
- Initialize a logger using `tsLogger aLogger("PATH//FILE_NAME.csv")` if you have a file path in mind or `tsLogger aLogger()` if you want to generate a new file automatically. The default file path is defined as `C:\\data` for the moment. You can use the member function `getFilePath()` to see where the file is located.
- If you want to append line(s) to a preamble, use `appendToPreamble("<SOME TEXT HERE>")`. You can do this as many times as you need. Note that any new line characters will be replaced by spaces by this call.
- `addPreamble()` adds the preamble to file. This includes a first line containing the total number of lines the preamble and header take together in the text file along with the UNIX time stamp when the library was initialized---separated by a comma. You can call this function only once and only before either adding the CSV header or starting to log data.
- Use the call `trackDataPoint(<pointer/address to datapoint>, "<OPTIONAL DATA NAME>")` to track a datapoint for logging. You must pass a pointer/address to said datapoint. Optionally, you may also pass a data name. If an empty string passed, tsLogger will auto-assign a name. You may track as many variables as you want. The supported types of data points are `int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, char, unsigned char, and bool`.
- `addDataHeader()` uses the names of the datapoints being tracked to append a CSV header into the text file. You can do this only once, and only before logging starts. If you have already added the preamble beforehand, it will modify the first line to indicate the extra line of headers before the CSV data begins.
- Whenever you need to log the state of all the tracked variables, simply call `logData()` to store the state of all of the variables as a line in your file.
- The first time you call this function (or manually if the user prefers), it calls `startLogger()` to indicate that loggging has started. This prevents the addition of preamble lines or a data header between the actual data as such a thing would corrupt the CSV data during ingestion.

In `./tsLogger_example/main.cpp`, you can find an example to get you started.

## Inspiration and learnings
As someone working on neuro-robotics, and near-real-time data acquisition and controls, I need to work on high performance code. This means, I usually work with C/C++. I needed to track dozens of datapoints that were being updated simultaneously every _1 ms_ with minimal performance penalty. The simple _"Let's put an `fprintf` outputing CSV lines to a file in the control loop"_ approach worked great until I started making PEBCAK errors like incorrect format specifiers or mistakes passing all datapoints to the right thread and function. I wanted a fool-proof way to collect this streaming real-time data so long as the performance penalty was not crushing. So, I created this library. Along the way, I learning about some of the new features that modern has to offer to offer like `std::variant`, `std::vector`, template functions and classes, and even lambdas.

I am now investigating replacing `std::visit` when resolving `std::variant` with `std::get_if` or my own implementation as I learned that `std::visit` is potentially slow [[1]](https://www.reddit.com/r/cpp/comments/kst2pu/with_stdvariant_you_choose_either_performance_or/), [[2]](https://www.reddit.com/r/cpp/comments/a8xkl3/when_performance_guarantees_hurts_performance/), [[3]](https://bitbashing.io/std-visit.html).

## LICENSE
This library is released under the permissive MIT license. So, please have at it!

## Feedback, issues and pull requests
Please don't hesitate to leave your feedback by opening up an issue. I welcome pull requests too for interesting features so long as the library remains dead-simple to use.
