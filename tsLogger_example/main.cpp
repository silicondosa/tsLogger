// tsLogger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <tsLogger.h>

int main()
{
    // library initialization
    std::cout << "Hello World!\n";
    char newFileName[] = "testFile.csv";
    tsLogger loggerA(newFileName);
        // if no file name is provided,
            // it will create one with the name "tsDataLog_YYYY-MM-DD-HHMM.csv"
    char filePathA[256];
    loggerA.getFilePath(filePathA, 256);
    std::cout<<"File path for log file A: "<<filePathA<<std::endl;

    // append things to the preabmle and add it to the text file
    loggerA.appendToPreamble("Test Preamble\n1"); // new line character is removed automatically
    loggerA.appendToPreamble("Test Preamble 2");
    loggerA.appendToPreamble("Test Preamble 3");
    loggerA.appendToPreamble("Test Preamble 4");
    loggerA.appendToPreamble("Test Preamble 5");
    loggerA.appendToPreamble("Test Preamble 6");
    loggerA.appendToPreamble("Test Preamble 7");
    loggerA.appendToPreamble("Test Preamble 8");

    loggerA.addPreamble();

    // test data points to link
    int a = -1;
    unsigned int b = 88;
    long c = -324684;
    unsigned long d = 684333;
    float e = (float) 2.718;
    double f = -53.2468143648136418;
    char g = 'g';
    unsigned char h = 254;
    bool i = true;
    long long j = -1314646343645;
    unsigned long long k = 1314646343645;

    // track data points
    loggerA.trackDataPoint(&a, "First data tracked");
    loggerA.trackDataPoint(&b, ""); // auto-labelled
    loggerA.trackDataPoint(&c, "datapoint 3");
    loggerA.trackDataPoint(&d, "data 4");
    loggerA.trackDataPoint(&e, "just a float");
    loggerA.trackDataPoint(&f, "double-precision sensor data");
    loggerA.trackDataPoint(&g, "data point 7");
    loggerA.trackDataPoint(&h, "another datapoint");
    loggerA.trackDataPoint(&i, "binary values also supported");
    loggerA.trackDataPoint(&j, "timestamp");
    loggerA.trackDataPoint(&k, "last data point");

    // add header to text file
    loggerA.addDataHeader();

    // log current state of data points
    loggerA.logData();

    // change state of data points
    a = -176;
    b = 564;
    c = -87130;
    d = 3418962;
    e = (float) - 3842.718;
    f = 0.123456789;
    g = 'G';
    h = 255;
    i = false;
    j = -9314646343645;
    k = 9314646343645;


    // log current state of data points
        // - call as many times as you need to log data states
    loggerA.logData();

    return 0;
}

