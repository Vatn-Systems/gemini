This repo contains the C++ interface driver for the Gemini sonar as well as the Tritech SDK source code. The driver is intended to setup the callback function for the Gemini, set some of the sonar parameters and read/save the sonar image data off of it. 

To run the driver:

1. run InstallSDK.sh to set up the symlinks (it might complain they arent there but they are (double check). This is likely from pushing the library code in a weird state)
2. cd into sandbox directory
3. mkdir sonarData
4. compile with g++ simpleDriver.cpp -lSvs5SeqLib -lGeminiComms -o simpleDriver
5. run simpleDriver
6. Sonar data is saved in csvs in the sonarData directory.
7. Control + C kills the program gracefully

Other notes:
- Gemini IP is 192.168.50.201
- For some reason the relative paths in the Tritech library don't make sense and I've had to update them manually for the libraries I wish to use.
- All of the code outside of the sandbox directory is from Tritech.



