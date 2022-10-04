#pragma once
#include <iostream>
#include <cmath>
#include <sndfile.h>
using namespace std;


// Creating a class called AudioFile, which will contain all local variables and functions.
class AudioFile
{
public:
    AudioFile();
    ~AudioFile();

    void read(string inputFileName);
    void write(string outputFileName);

    void analysis();
    void details();


    void scaleSignal(float scalingFactor);
    void compressor(float Gain, float Threshold, bool soft_knee, int ratio, double attack, double release);
    void delay(double delay_sec);

private:
    int len;
    int numChannels;
    int fs;
    float* inputBuffer;
    string file;
    float* outputBuffer;

    float* compressor_buffer;
    float* compressor_buffer_knee;
    float* compressor_buffer_computer_gain;
    float* compressor_buffer_smoothed;
    float* makeup_gain;

    float sum = 0;
    float mean = 0;
    float rms = 0;
};


//Other Functions
void exit_func(string x, bool &error, bool& y);