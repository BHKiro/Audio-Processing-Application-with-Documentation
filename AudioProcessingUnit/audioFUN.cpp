#include "audioFUN.h"

using namespace std;

//Allocating memory
AudioFile::AudioFile()
{
    inputBuffer = NULL;
    len = 0;
    numChannels = 0;
    fs = 0;
    file = "";
} 

//Deallocating memory
AudioFile::~AudioFile()
{
    if (inputBuffer != NULL)
    {
        delete[]inputBuffer;
    }
}

//Read function
void AudioFile::read(string fileName)
{
    file = fileName; 

    // Following lines read audio files header
    SF_INFO fileInfo;
    SNDFILE* sndFile = sf_open(fileName.c_str(), SFM_READ, &fileInfo);

    // Checking if the file was read correctly.
    if (sndFile == NULL)
    {
        cout << "Error reading file..." << endl;
    }
    else
    {
        if (inputBuffer != NULL)
        {
            // In case a file is read in more than once
            delete[] inputBuffer;
        }
        inputBuffer = new float [fileInfo.frames]; //Creating a new array that has the same values as the input file

        sf_readf_float(sndFile, inputBuffer, fileInfo.frames);

        // Setting the other audio file properties
        len = (int)fileInfo.frames;
        fs = fileInfo.samplerate;
        numChannels = fileInfo.channels;
    }
    sf_close(sndFile);
}

//Write function
void AudioFile::write(string outputFileName)
{
    // Creating output file settings (16bit Mono PCM)
    SF_INFO info;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    info.channels = numChannels;
    info.samplerate = fs;

    SNDFILE* sndFile = sf_open(outputFileName.c_str(), SFM_WRITE, &info);

    // Checking if the file can be written.
    if (sndFile == NULL)
    {
        cout << "Error: Could not write file " << endl;
    }
    else
    {
        sf_writef_float(sndFile, outputBuffer, len);
    }

    sf_write_sync(sndFile);
    sf_close(sndFile);
}

//Analysis function
void AudioFile::analysis()
{
    //Calculates Sum
    for (int i=0; i < len; i++)
    {
        sum = sum + inputBuffer[i];
    }

    //Calculates Mean
    mean = sum / (float)len;

    //Calculates RMS
    for (int i = 0; i < len; i++)
    {
        sum = sum + (inputBuffer[i] * inputBuffer[i]);
    }
    mean = sum / (float)len;
    rms = sqrt(mean);
}

//Details function
void AudioFile::details()
{
    //Checking if the file was loaded correctly
    if (inputBuffer != NULL)
    {
        //Printing out all the perks of an audio file.
        cout << "The file that you have entered has those features: " << endl;
        cout << "FileName: " << file << endl;
        cout << "Length: " << (float)len / fs << "s" << endl;
        cout << "Samplerate: " << fs << endl;
        cout << "Channels: " << numChannels << endl;
        cout << "RMS: " << rms << endl;
    }
    else
    {
        cout << "Error, file not loaded" << endl;
    }

    outputBuffer = inputBuffer; //In case user wants to just look at file properties.
}

//Scale Signal function
void AudioFile::scaleSignal(float scalingFactor)
{
    //Creating a new outputBuffer
    outputBuffer = new float[len];

    // Simple "for" loop used to scale each frame of the signal.
    for (int i = 0; i < len; i++)
    {
        outputBuffer[i] = inputBuffer[i] * scalingFactor;
    }

};

//Dynamic Compressor function
void AudioFile::compressor(float Gain, float Threshold, bool soft_knee, int ratio, double attack, double release)
{
    //Declaring and creating all needed variables and arrays
    compressor_buffer = new float [len];
    compressor_buffer_knee = new float[len];
    compressor_buffer_computer_gain = new float[len];
    compressor_buffer_smoothed = new float[len];
    makeup_gain = new float[len];
    outputBuffer = new float[len];

    double knee_width;
    double r = ratio;

    //Calcucalting Attack time and Release time coefficients
    double attack_coeff = exp((-log(9)) / (fs * attack));
    double release_coeff = exp((-log(9)) / (fs * release));

    // The input signal (inputBuffer) gets converted to decibels:
    for (int i = 0; i < len; i++)
    {
        if (inputBuffer[i] >= 0) {
            compressor_buffer[i] = 20 * log10(inputBuffer[i]);
        }
        else if (inputBuffer[i] < 0) {
            compressor_buffer[i] = 20 * log10(abs(inputBuffer[i])); //An error occured when the program tried to calculate the negative values. (It basically did output imaginary numbers)
                                                                    //To work around this it was decided to just convert all the negative values to the positive ones using "abs" function
                                                                    //from <cmath>
        }
    }


   // 1 Applying Gain 
   // Basic scaling function, but with decibels.
    for (int i = 0; i < len; i++) 
    {
        compressor_buffer[i] = compressor_buffer[i] + Gain;
    }


    // 2 Applying Treshold
    //This function attenuates signal that is above the threshold. All the equations are taken from various sources describing how the signals gets altered on different steps of a compressor.
    if (soft_knee == true)
    {
        cout << "Please insert knee width in dB: ";
        cin >> knee_width;
        double knee_neg = Threshold - (knee_width / 2);
        double knee = Threshold + (knee_width / 2);
        for (int i = 0; i < len; i++)
        {
            if (compressor_buffer[i] >= knee_neg && compressor_buffer[i] <= knee)
            {
               compressor_buffer_knee[i] = compressor_buffer[i] + ((((1 / r) - 1) * (compressor_buffer[i] - knee)) / (2 * knee_width));
            }
            else if (compressor_buffer[i] > knee)
            {
                compressor_buffer_knee[i] = Threshold + ((compressor_buffer[i] - Threshold) / r);
            }
            else if (compressor_buffer[i] < knee_neg) 
            {
                compressor_buffer_knee[i] = compressor_buffer[i];
            }
        }
    }
        else if (soft_knee == false)
        {
            for (int i = 0; i < len; i++)
            {
                if (compressor_buffer[i] >= Threshold)
                {
                    compressor_buffer_knee[i] = Threshold + ((compressor_buffer[i] - Threshold) / r);
                }
                else {}
            }
        }



    //3 Computed gain 
    //The difference between attenuated signal and the input signal in dB
    for (int i = 0; i < len; i++)
    {
        compressor_buffer_computer_gain[i] = compressor_buffer_knee[i] - compressor_buffer[i];
    }


    compressor_buffer_smoothed = compressor_buffer_computer_gain; //Assigning the same value to an array, so that the smoothing can be done.

    //4 Smoothing
    // Computed gain then is smoothed using specified attackand release time parameters.
    for (int i = 1; i < len; i++)
    {
        if (compressor_buffer_computer_gain[i] <= compressor_buffer_smoothed[i-1])
        {
            compressor_buffer_smoothed[i] = ((attack_coeff * compressor_buffer_smoothed[i - 1]) + ((1 - attack_coeff) * compressor_buffer_computer_gain[i]));
        }
        else if (compressor_buffer_computer_gain[i] > compressor_buffer_smoothed[i - 1])
        {
            compressor_buffer_smoothed[i] = (release_coeff * compressor_buffer_smoothed[i - 1]) + ((1 - release_coeff) * compressor_buffer_computer_gain[i]);
        }
    }

    //5 Make-up gain
    for (int i = 0; i < len; i++)
    {
        if (compressor_buffer[i] == 0)
        {
            makeup_gain[i] = abs(compressor_buffer_knee[i]);
        }
        else
        {
            makeup_gain[i] = 0;
        }
    }

    //The make-up gain is added to the smoothed gain.
    for (int i = 0; i < len; i++)
    {
        compressor_buffer_smoothed[i] = compressor_buffer_smoothed[i] + makeup_gain[i];
    }
   


    //7 The calculated gain in dB is translated to a linear domain.
    for (int i = 0; i < len; i++)
    {
        compressor_buffer_smoothed[i] = pow(10, (compressor_buffer_smoothed[i] / 20));
    }

  

    //8 The output of the dynamic range compressor is assigned below.
    for (int i = 0; i < len; i++)
    {
        outputBuffer[i] = inputBuffer[i] * compressor_buffer_smoothed[i];
    }
}

//Delay function
void AudioFile::delay(double delay_sec)
{
    //Calculating the delay in samples
    int delay = delay_sec * fs;

    //Assigning new value to variable "len"
    len = len + delay;
    outputBuffer = new float[len];


    //Delay function
    for (int i = 0; i < len; i++)
    {
        if (i < delay)
        {
            outputBuffer[i] = inputBuffer[i];
        }
        else if (i >= delay && i < len)
        {
            outputBuffer[i] = inputBuffer[i] + inputBuffer[i - delay];
        }
        else
        {
            outputBuffer[i] = inputBuffer[i - delay];
        }
    }
}


//Exit function
void exit_func(string x, bool& error, bool& y) {
    if (x == "Y" || x == "y")
    {
        y = true;
        error = false;
    }
    else if (x == "N" || x == "n")
    {
        y = false;
        error = false;
    }
    else {
        cout << "invalid input, try again" << endl;
        error = true;
    }
}