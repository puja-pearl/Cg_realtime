# Multi Server System
multi server alient system with CRC check implimentation using networking in systems C and socket API


# Objective 
need to create a client server system in c and Socket API that displays data in Qt interface
there should be 2 servers and 3 clients 

## server 

server_1 should be referenced as a master server and other (server_2) will be as slave server
both server will check for their health status in every 2 seconds 
recieving the generated data from the client master server will do a CRC check and recieve 5 such entries and cosolidate data in single file and send it to the Qt interface to display there
if master fails slave will work as master server


## client 
client will generate a 3 digit random number in every 1 second
it fetch the timestamp from the system and save that into the file (20 entires at a time)
and the send it to both the servers with a CRC check
