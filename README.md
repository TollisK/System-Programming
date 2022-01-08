# System Programming - Vaccine Monitor App
This project implements a vaccine monitor app with one client and miltiple servers.
<br>
### Step by step
For the implementation of the first project there is a [bloom filter](https://en.wikipedia.org/wiki/Bloom_filter#:~:text=A%20Bloom%20filter%20is%20a,a%20member%20of%20a%20set.), a [skip list](https://en.wikipedia.org/wiki/Skip_list#:~:text=Skip%20lists%20are%20a%20probabilistic,faster%20and%20use%20less%20space.) and 2 linked lists country and person which stores data for every person and country we encountered.
<br/>
While for the second project there are two new files Monitor and travelMonitor that will act as the server and client. For the connection and synchronization of these two it uses FIFO pipes and signals to send data from one file to another.
<br/>
In the third and last project there are miltiple server-monitors and only one client-travelMonitor which connect via POSIX threads and sockets.

## Bash script testfile.sh:
This bash script creates a test input filecalled "inputFile.txt".

The input file will contain a number of citizen vaccination records about different viruses, in the following order:
`citizenId firstName lastName country age virusName isVaccinated dateVaccinated`

For example, <br/>
    `125 Jason Dupont USA 76 H1N1 YES 30-10-2020`<br/>
    `889 John Papadopoulos Greece 52 Η1Ν1 ΝΟ`
    
### Create the input file:
    bash ./testFile.sh virusesFile.txt countriesFile.txt numLines duplicatesOn

Where **numLines** equals with the number of linesthat will be in the "inputFile.txt" and **duplicatesOn** = 1 if duplicate citizenIDs are allowed, or 0 if citizenIDs are unique.

The **virusesFile.txt**, located in the info directory, contains a list of 20 different viruses that can be modified as you wish. Same way with the **countriesFile.txt**.

## Bash script: create_infiles.sh

This bash script reads the inputFile.txt and creates a directory that contains subdirectories, one for each country name that exists in the inputFile. In each subdirectory, there are  **numFilesPerDirectory** files with names **country-n.txt**, where n is a positive integer. 


## Execution:

To run the program there is a makefile which compiles the files in an executable named travelMonitorClient. Once an executable file has been created, we call it with the command:

    ./travelMonitor -b X -s Y -i Z -m K -c C -t T

where X, Y, Z, K are the parameters for its size buffersize (with a default value of 200 bytes), the size of the bloom filter (default value 10000 bytes), the name of the data folder and the number of the process Monitor we want to run (default value 2) respectively. Also C and T denote the size of the circular buffer while T denotes the number of threads.
It then waits for input from the user to execute a command.
