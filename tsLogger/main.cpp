// tsLogger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <tsLogger.h>

int main()
{
    std::cout << "Hello World!\n";
    char newFileName[] = "C:\\data\\testFile.csv";
    tsLogger LoggerA(newFileName);
    //tsLogger LoggerA;
    char filePathA[256];
    LoggerA.getFilePath(filePathA, 256);
    std::cout<<"File path for log file A: "<<filePathA<<std::endl;

    LoggerA.appendToPreamble("Test Preamble\n1");
    LoggerA.appendToPreamble("Test Preamble 2");
    LoggerA.appendToPreamble("Test Preamble 3");
    LoggerA.appendToPreamble("Test Preamble 4");
    LoggerA.appendToPreamble("Test Preamble 5");
    LoggerA.appendToPreamble("Test Preamble 6");
    LoggerA.appendToPreamble("Test Preamble 7");
    LoggerA.appendToPreamble("Test Preamble 8");

    LoggerA.addPreamble();

    LoggerA.addDataHeader();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
