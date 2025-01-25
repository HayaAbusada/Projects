/*
    Author of the starter code
    Yifan Ren
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 9/15/2024

    Please include your Name, UIN, and the date below
    Name: Haya Abusada
    UIN: 232006631
    Date: 09/17/2024
*/


#include "common.h" 
#include "FIFORequestChannel.h"  
#include <iostream>  
#include <fstream>  
#include <cstdlib>  
#include <cstring>  
#include <unistd.h> 
#include <sys/types.h>  
#include <sys/wait.h>  

using namespace std;  

int main(int argc, char *argv[]) {
    int opt;  // Variable to store options from command-line arguments
    int p = 1;  // Default patient ID is 1
    double t = 0.0;  // Default time is 0.0
    int e = 1;  // Default ECG channel is 1
    string filename = "";  // Filename is empty by default (no file requested)
    bool collect_data = false;  // Flag to determine if data collection is requested

    // Parse command line arguments using getopt
    while ((opt = getopt(argc, argv, "p:t:e:f:c")) != -1) {
        switch (opt) {
            case 'p':  // Patient ID option
                p = atoi(optarg);  // Convert the patient ID to an integer
                break;
            case 't':  // Time option
                t = atof(optarg);  // Convert the time to a double
                break;
            case 'e':  // ECG channel option
                e = atoi(optarg);  // Convert the channel to an integer
                break;
            case 'f':  // File option
                filename = optarg;  // Store the filename
                break;
            case 'c':  // Data collection flag option
                collect_data = true;  // Set the collect data flag to true
                break;
        }
    }

    // Task 1: Fork a process to run the server as a child process
    pid_t pid = fork();  // Fork the current process
    if (pid < 0) {
        perror("Fork failed");  // Error handling for failed fork
        exit(EXIT_FAILURE);  // Exit the program with a failure code
    } else if (pid == 0) {
        // This is the child process
        // Execute the server program
        execl("./server", "./server", nullptr);  // Replace the child process with the server
        perror("execl failed");  // Error handling for failed execution
        exit(EXIT_FAILURE);  // Exit if execl fails
    } else {  // This is the parent process (Client process)
        // Create a communication channel with the server
        FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);

        // Task 2: Collect data if the collect_data flag is set
        if (collect_data) {
            cout << "Collecting data..." << endl;  // Inform the user that data collection is starting

            // Open a file to write collected data (CSV format)
            ofstream outfile("received/x1.csv");
            if (!outfile.is_open()) {
                cerr << "Failed to open x1.csv for writing" << endl;  // Error handling if file cannot be opened
                return 1;  // Exit the program with an error code
            }

            double ecg1, ecg2;  // Variables to store ECG values
            for (int step = 0; step < 1000; ++step) {
                // Request data for ECG channel 1
                datamsg request1(p, 0.004 * step, 1);  // Create a datamsg for ECG channel 1
                chan.cwrite((char*)&request1, sizeof(datamsg));  // Write the request to the channel
                chan.cread(&ecg1, sizeof(double));  // Read the ECG value from the server

                // Request data for ECG channel 2
                datamsg request2(p, 0.004 * step, 2);  // Create a datamsg for ECG channel 2
                chan.cwrite((char*)&request2, sizeof(datamsg));  // Write the request to the channel
                chan.cread(&ecg2, sizeof(double));  // Read the ECG value from the server

                // Write the results to the CSV file
                outfile << step * 0.004 << "," << ecg1 << "," << ecg2 << endl;
            }
            outfile.close();  // Close the CSV file after writing
        } 
        // If filename is not provided and data collection is not requested
        else if (filename.empty() && !collect_data) {
            ofstream outfile("received/x1.csv");  // Open the file to write the ECG data
            if (!outfile.is_open()) {
                cerr << "Failed to open x1.csv for writing" << endl;  // Error handling if file can't be opened
                return 1;  // Exit with an error code
            }

            // Request ECG data points for both channels
            for (int step = 0; step < 1000; ++step) {
                // Request data for ECG channel 1
                datamsg request(p, 0.004 * step, 1);  // Create a datamsg for ECG channel 1
                char buf[MAX_MESSAGE];  // Buffer to store the message
                memcpy(buf, &request, sizeof(datamsg));  // Copy the datamsg to the buffer
                chan.cwrite(buf, sizeof(datamsg));  // Send the request to the server
                double value1;
                chan.cread(&value1, sizeof(double));  // Read the ECG value for channel 1

                // Request data for ECG channel 2
                request = datamsg(p, 0.004 * step, 2);  // Create a datamsg for ECG channel 2
                memcpy(buf, &request, sizeof(datamsg));  // Copy the datamsg to the buffer
                chan.cwrite(buf, sizeof(datamsg));  // Send the request to the server
                double value2;
                chan.cread(&value2, sizeof(double));  // Read the ECG value for channel 2

                // Write the values to the CSV file
                outfile << step * 0.004 << "," << value1 << "," << value2 << endl;
            }
            outfile.close();  // Close the file after writing
        }

        // Task 3: Request a file from the server if the filename is provided
        if (!filename.empty()) {
            // Request the size of the file first
            filemsg fm(0, 0);  // Create a filemsg to request the file size
            int len = sizeof(filemsg) + filename.size() + 1;  // Calculate the message length
            char* buf = new char[len];  // Allocate a buffer to hold the message
            memcpy(buf, &fm, sizeof(filemsg));  // Copy the filemsg to the buffer
            strcpy(buf + sizeof(filemsg), filename.c_str());  // Copy the filename to the buffer
            chan.cwrite(buf, len);  // Send the request to the server

            __int64_t file_length;  // Variable to store the file length
            chan.cread(&file_length, sizeof(__int64_t));  // Read the file length from the server
            std::cout << "The length of " << filename << " is " << file_length << endl;

            // Create the received directory if it doesn't exist
            system("mkdir -p received");  // Create the directory for the received file
            string output_filename = "received/" + filename;  // Construct the output filename

            // Open the output file to write the received file data
            ofstream outfile(output_filename, ios::binary);
            if (!outfile.is_open()) {
                cerr << "Failed to open " << output_filename << " for writing" << endl;  // Error handling if the file can't be opened
                delete[] buf;  // Free the allocated buffer
                return 1;  // Exit with an error code
            }

            // Transfer the file in chunks
            __int64_t offset = 0;  // Initialize the offset for reading the file
            __int64_t chunk_size = MAX_MESSAGE - sizeof(filemsg);  // Calculate the maximum chunk size

            while (offset < file_length) {
                // Determine the number of bytes to read in this chunk
                __int64_t bytes_to_read = min(chunk_size, file_length - offset);  // Read the smaller of the chunk size or remaining bytes
                fm = filemsg(offset, bytes_to_read);  // Create a filemsg with the current offset and size
                memcpy(buf, &fm, sizeof(filemsg));  // Copy the filemsg to the buffer
                chan.cwrite(buf, len);  // Send the request to the server

                char recv_buffer[MAX_MESSAGE];  // Buffer to store the received data
                chan.cread(recv_buffer, bytes_to_read);  // Read the file chunk from the server
                outfile.write(recv_buffer, bytes_to_read);  // Write the chunk to the output file
                offset += bytes_to_read;  // Increment the offset by the number of bytes read
            }

            outfile.close();  // Close the output file after writing
            delete[] buf;  // Free the allocated buffer
            std::cout << "File " << filename << " has been received." << endl;
        }

        // Task 4: Request a new channel from the server
        MESSAGE_TYPE new_channel = NEWCHANNEL_MSG;  // Message type for requesting a new channel
        chan.cwrite(&new_channel, sizeof(MESSAGE_TYPE));  // Send the request for a new channel

        char new_channel_name[100];  // Buffer to store the new channel name
        chan.cread(new_channel_name, sizeof(new_channel_name));  // Read the new channel name from the server
        FIFORequestChannel new_chan(new_channel_name, FIFORequestChannel::CLIENT_SIDE);  // Create a new channel for communication

        // Request data points using the original channel
        char buf[MAX_MESSAGE];  // Buffer to store the message
        datamsg x(1, 0.0, 1);  // Create a datamsg to request ECG data
        memcpy(buf, &x, sizeof(datamsg));  // Copy the datamsg to the buffer
        chan.cwrite(buf, sizeof(datamsg));  // Send the request to the server
        double reply;  // Variable to store the server's response
        chan.cread(&reply, sizeof(double));  // Read the response from the server
        cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;

        // Use the new channel to request data
        datamsg x_new(p, t, e);  // Create a datamsg to request data from the new channel
        new_chan.cwrite((char*)&x_new, sizeof(datamsg));  // Send the request via the new channel
        double reply_new;  // Variable to store the server's response from the new channel
        new_chan.cread(&reply_new, sizeof(double));  // Read the response from the new channel
        std::cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply_new << endl;

        // Task 5: Close all channels by sending a QUIT message
        MESSAGE_TYPE m = QUIT_MSG;  // Message type for quitting
        chan.cwrite(&m, sizeof(MESSAGE_TYPE));  // Send the quit message to the original channel
        new_chan.cwrite(&m, sizeof(MESSAGE_TYPE));  // Send the quit message to the new channel

        // Wait for the server process to terminate
        wait(NULL);  // Wait for the child (server) process to finish
    }

    return 0;  // Exit the program successfully
}








