/*

Kirills Zarubins
Audio Software Development 
DIG4150

Audio Processing Portable Unit (APPA)
*/
#include <iostream>
#include "audioFUN.h"
#include "ms_getenv.h"
#include <sndfile.h>
#include <cmath>
#include <string>
using namespace std;

int main()
{
    //Declaring all needed variables
    float Vol; // A variable for a scale funcion
    bool exit; // Adding an exit bool variable to later on give user a choice to exit the program or continue.
    int choice;// A value that is going to be used for a switch operator below

    bool error_switch;// Bool variable that is being used by do->while function to keep looping until user inputs anything that fits

    bool error_main; // Bool variable that is being used by do->while function to keep looping until user inputs Y/N
    string x; // Variable to store user input


    //Getting a path to the file using an ms_getenv function to get the home directory provided by Nick.
    string homeDir = ms_getenv("USERPROFILE");
    string inputFile = homeDir + "/Desktop/nobody.wav";
    string outputFile = homeDir + "/Desktop/nobody_processed.wav";

    //Naming the class with functions 
    AudioFile audio;
    
    //Reading Input File
    audio.read(inputFile);

    //Analysing Input File. Calculating useful infromation, like RMS, Mean and Sum.
    audio.analysis();


    /*
    Onward there will be all functions that are in this program, user also has a choice of things he wants to do to the file.
    */
    
    //The actual start
    do {
    //Main do->while function (this one is used to exit the program when user decides that he wants to)
        cout << "Welcome to APPA, please decide what would you like to do" << endl;
        cout << endl << "A list of functions available: " << endl;
        cout << "1 - Details (Prints out the details of an input file) " << endl << "2 - Scale (Changes the volume of a file) " << endl
            << "3 - Compressor (Scales the signal and does not allow it to be louder than a threshold) " << endl << "4 - Delay " << endl;
            cout << "Please insert what would you like to do with your file (1 - 4): "; //This is going to print this line into the console
            /* 
            Following do->while function is going to loop until user inputs a proper value(until "error_switch" is equal to 0(false)). 
            The hardest thing to accomplish was to restrict the user from inserting a character or a string. 
            Switch operator automatically switches between numbers, and goes to "default" if it could not find a value that coorelates to a specific "case". So that means,
            that whenever user inputs something other than 1,2,3,4... it will go to the "default" case.
            When user decides to input something other than an integer, an "int choice" flags an error, but does not break the code so I had to make sure that 
            "cin >> choice" does not contain errors and is ready to be reused. To be continued under "default" case...
            */
            do // Main do-> while loop, which is giving the program to check if user wants to exit the program.
            {
            cin >> choice; //User input of choice
            switch (choice) /* Switch is an operator of multiple choice which allows the program to jump between a lot of instances. This operator was used, because 
                               it is a lot quicker and easier than writing an "if" statement for every function that user may want to call.
                            */
            {
            case 1: //Choice 1 includes a function, which prints out all the details about the file.
            {
                cout << endl << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>DETAILS<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
                //Showing details
                audio.details();

                error_switch = 0; //This variable is indicating that there are no errors in this "case".
                break; //This "break" operator does not allow the program to go to the next "case", it just jumps straight out of the "switch" operator 
            }
            case 2: //Choice 2 includes a simple function that allow user to change the volume of an audio file.
            {
                //Volume Change function with user input
                cout << endl << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>SCALE<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
                cout << "Please insert the volume you would like your audio file to be(0 - 100): "; //Prints out this line into the console

                cin >> Vol; // Assigns user input to a "Vol" variable

                Vol = Vol * 0.01; // Sound wave in digital world is just an array of numbers. Those numbers vary from -1 to 1, which is also often refered to Voltage Peaks.
                                  // As it is seen above, this program asks user to insert a value from 0 to 100, which is meant to be a precentage value. 
                                  // This line converts the value, so that it can be correctly used in the following function. 

                audio.scaleSignal(Vol); //This line calls a function by the name of "scaleSignal"
                error_switch = 0; //Again, this just assign a "False" value to the error_switch variable, so that the program can exit do->while loop if everything is fine.
                break; // "Break" operator
            }
            case 3: //Choice 3 includes an advanced function that can be described as a dynamic compressor.
            {
                cout << endl << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>COMPRESSOR<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
                //Declaring all the variables
                float comp_dB, comp_Thresh;
                double attack, release;
                int ratio;
                string knee_str;
                bool knee;
                int comp_switch = 1; //This variable is used for going through every "case"
                bool comp_error = 1; //This variable indicates whether there is an error or not

                /*
                This dynamic compressor is the main part of this code, becuase it took a lot of effort and hours to complete. 
                  Firstly, a lot of research had to be done on how compressors work and how does every value introduced above and below affect the signal. It was particularly hard to 
                find a decent source of information which contained everything that was needed for this to code. In the end, almost when there was no hope to find anything useful, a 
                good old MATLAB saved the day. Figuring out how to code a dynamic compressor from bits and pieces scattered around the internet was a hard tast, but this MathWorks
                tutorial on how their compressor works - https://www.mathworks.com/help/audio/ref/compressor.html#bu_1477-1-Attacktimes was used as a framework for this dynamic compressor. 
                  Secondly, another hard thing to achieve was the error checking in this particular bit of code. Earlied error checking was done using "switch" operator and its useful case
                called "default" which gets called whenever there's something wrong with a "switch" variable. Here, on the other hand, it was not as broad of an error. So each individual input
                had to be checked and if something was out of boundaries, the program had to then jump back to the same case and ask the user to insert the value again.
                */
                do //do->while loop which is responsible for errors in user inputs.
                {
                    switch (comp_switch) // "Switch" opertor which will loop through every case using an interesting technique.
                    {
                    case 1: // Case 1: Gain
                    {
                        cout << "Please insert gain in dB (0.1 - 40): "; // This line asks user to input gain. 0 gain could not be done, due to the bug, that did occur with error checking
                        cin >> comp_dB; // Assigns a value to comp_dB
                        if (comp_dB < 0 || comp_dB > 40 || comp_dB == NULL) // This "if" statement will check if user input is not within given boundaries or is not a numeric value at all.
                                                                            // Due to its nature, an error that occurs when user inputs characters can be seen as a NULL, which is also a 0.
                                                                            // So that is the reasoning behind not being able to insert 0 as a gain value.
                        {
                            cout << "You have inserted something that does not fit, please try again." << endl;
                            cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than an integer
                            cin.ignore(1000000, '\n'); // ignores the next 1000000 characters, hoping that user didnt insert anything bigger than that, then jumps onto the next line.
                            comp_switch = 1; //Assigns a value of 1 to a variable "comp_switch". This is done to then come back to the same case, so that the user can try again.
                            break; //Jumps to the end of an operator.
                        }
                        else // ... If everything is correct and within given boundaries
                        {
                            comp_switch = 2; //Assigns a value of 2 to a variable "comp_switch" so that the program will go to the second case on the next loop.
                            break;
                        }
                    }
                    case 2: // Case 2: Threshold
                    {
                        cout << "Please insert threshold in dB: ";
                        cin >> comp_Thresh;
                        //Same thing again. Due to used error catching technique - user cannot insert a value of 0.  
                       
                        if (comp_Thresh == NULL)
                        {
                            cout << "You have inserted something that does not fit, please try again." << endl;
                            cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than an integer
                            cin.ignore(10000, '\n'); // ignores the next 1000000 characters, hoping that user didnt insert anything bigger than that, then jumps onto the next line.
                            comp_switch = 2; // Coming back to the same case.
                            break;
                        }
                        else 
                        {
                            comp_switch = 3; // The next case under the number of 3.
                            break;
                        }
                    }
                    case 3: // Case 3: Ratio
                    {
                        // Compression ratio is the input/output ratio for signals that overshoot the operation threshold.
                        cout << "Please insert ratio: ";
                        cin >> ratio;
                        if (ratio == NULL)
                        {
                            cout << "You have inserted something that does not fit, please try again." << endl;
                            cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than an integer.
                            cin.ignore(10000, '\n'); // ignores the next 1000000 characters, hoping that user didnt insert anything bigger than that, then jumps onto the next line.
                            comp_switch = 3; //Stays the same
                            break;
                        }
                        else {
                            comp_switch = 4; //Goes to the next one
                            break;
                        }
                    }
                    case 4: //Case 4: Attack time
                    {
                        // Attack time is the time the compressor gain takes to rise from 10% to 90% of its final value when the input goes above the threshold. 
                        // The Attack time (s) parameter smooths the applied gain curve.
                        cout << "Please insert attack in seconds: ";
                        cin >> attack;
                        if (attack == NULL)
                        {
                            cout << "You have inserted something that does not fit, please try again." << endl;
                            cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than an integer
                            cin.ignore(10000, '\n'); // ignores the next 1000000 characters, hoping that user didnt insert anything bigger than that, then jumps onto the next line.
                            comp_switch = 4; //Keeps the number the same
                            break;
                        }
                        else {
                            comp_switch = 5; //Goes onto the next one
                            break;
                        };
                    }
                    case 5: // Case 5: Release time
                    { 
                        // Release time is the time the compressor gain takes to drop from 90 % to 10 % of its final value when the input goes below the threshold.
                        // The Release time(s) parameter smooths the applied gain curve.
                        cout << "Please insert release in seconds: ";
                        cin >> release;
                        if (release == NULL)
                        {
                            cout << "You have inserted something that does not fit, please try again." << endl;
                            cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than an integer
                            cin.ignore(10000, '\n'); // ignores the next 1000000 characters, hoping that user didnt insert anything bigger than that, then jumps onto the next line.
                            comp_switch = 5;
                            break;
                        }
                        else {
                            comp_switch = 6;
                            break;
                        }
                    }
                    case 6: // Case 6: Soft knee/Hard knee
                    {
                        // The bool variable was another issue that occured when trying to implement error catching. Only because for the error catching NULL was used, it meant that
                        // the false part of bool variable was not even accesible, so here is what has been done:
                        cout << "Please insert if you would like a soft knee or a hard knee(1 - soft / 0 - hard): ";
                        cin >> knee_str; // It was decided to read user input as a string, to then implement needed value.
                        if (knee_str == "1" || knee_str == "soft" || knee_str == "Soft" || knee_str == "SOFT") // As well as having an input that equals to "1", program now recognises values
                                                                                                               // like "soft" if user decides to write them. Just a neat little thing that won't
                                                                                                               // hurt anybody.
                        {
                            knee = 1; //Assigns "true" value to variable "knee"
                            comp_switch = 7;
                            break;
                        }
                        else if (knee_str == "0" || knee_str == "hard" || knee_str == "Hard" || knee_str == "HARD") // Same is done here, but with number "0" and word "hard"
                        {
                            knee = 0;//Assigns "false" value to variable "knee"
                            comp_switch = 7;
                            break;
                        }
                        else 
                        {
                            cout << "You have inserted something that does not fit, please try again." << endl;
                            cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than an integer
                            cin.ignore(10000, '\n');
                            comp_switch = 6;
                            break;
                        }
                    }
                    }
                } while (comp_switch <= 6 && comp_error == 1); // This do->while operator will loop until a variable "comp_switch" is bigger than 6 and the code has no errors.
                //Calling a Compressor function
                audio.compressor(comp_dB, comp_Thresh, knee, ratio, attack, release);
                error_switch = 0; // Says that there were no errors in this case.
                break; // Jumps to the end.
            }
            case 4: // Choice 4 includes a Delay function.
            {
                cout << endl << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>DELAY<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
                //Declaring variables
                double delay_sec;
                bool delay_err = 1; // Bool value for errors
                do {
                    cout << "Please insert delay in seconds: ";
                    cin >> delay_sec;
                    if (delay_sec == NULL || delay_sec < 0) // If user input is less than 0 or doesn't contain numbers it will go through here.
                    {
                        cout << "You have inserted something that doesn't fit. Please try again." << endl;
                        cin.clear(); // Clearing "cin" out of errors, if user tried to input something other than a number.
                        cin.ignore(10000, '\n');
                        delay_err = 1; // Saying that there was an error.
                    }
                    else if (delay_sec > 0) // If everything is fine it will continue to here.
                    {
                        audio.delay(delay_sec);
                        delay_err = 0; // Saying that everything went fine.
                    }
                } while (delay_err == 1); // This will loop in case there was an error
                error_switch = 0; //Declaring that there were no errors
                break;
            }
            default: // Program will go to here if there was an error with user input in the beginning.
            {
                cout << "You have inserted something that doesn't fit. Please make sure to correctly write an integer related to a function: ";
                cin.clear(); // Clearing "cin>>choice" out of errors, because if user tried to input something other than an integer
                cin.ignore(10000, '\n');
                error_switch = 1; //Declaring that there was an error
            }
            }
            } while (error_switch == 1); //This will loop until an error variable is set to 0.

            // Asking if user wants to do something else with a little input error checking.
            do {
                cout << "Would you like to do something else?(Y/N)" << endl; 
                cin >> x;
                exit_func(x, error_main, exit); // calls an exit function.
            } while (error_main == true); //This will loop until error_main is set to false
    } while (exit == true); //This will loop until error_main is set to false


    //Outputing File
    audio.write(outputFile);
    cout << "Your file is located in " << outputFile << endl;


    return 0;
}
