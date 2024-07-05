Street Light Automation and Fault Tolerance system

The aim of the project is to minimize cost , monitor electricity consumption and faults on a regular basis and control all street lamps in real time from a central server.

The setup is such that there will be single microcontroller in control of a set of 10-15 street lamps each separated by 50m at least. Each of these microcontroller is in turn controlled by local server which will be present in the locality and further these local servers are controlled by a central server.
Each of the street lamp will contain a NodeMCU containing an ESP8266 which will help in collecting data such as whether it is on or off , light intensity etc on regular interval. 
The NodeMcu can work in two modes, Access point method ( AP) and station mode (ST) similar to client and server architecture.
Each of the NodeMCU can act as AP and ST to communicate with each other without need of internet , and data from each AP will be finall collected at the central microcontroller.

 We have assumed that Internet connection will be available for the central microcontroller so that we send the data to local server using HTTP GET/POST methods.

 The next milestone to cover will be simulation of realtime implementation in a smaller scale without much attention to realtime synchronization but to ensure establishment of communication between ESP8266 devices.
This will be done using two NodeMCU's with a LED light and a sample server will be used to collect data and stored in backend with timestamp.


