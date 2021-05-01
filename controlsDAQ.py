import matplotlib.pyplot as plt
import serial
from serial import Serial
import csv
import time
import datetime

# TO DO
# Graph time (comes in from serial as the X parameter in X, Y)
# Adjust time values to only have seconds(?)
# Make it easy for alex to run code on his laptop

# The code below creates a live plot that plots in front of us.
# Adjust as needed: x and y axes (voltage up to 600, ec)
                  # pressure data
                  # currently, voltage = dummy data from arduino
                  # with the timeInc/time additions, this graph plots the dummy
                         # arduino data versus time (x, increments of 1)
                         # cc=str(ser.readline()) and cc[2:][:-5] display live data
                              # from arduino

#                   ymax should be 600 volts for voltage
#plt.axis([0, 100, 0, 600])
# If we don't set axis beforehand, the graph will adjust axes as needed, which is great


# can't pass by ref
def purify(string):

    for x in range(0, len(string)):
        if string[x] != '0' or string[x] != '1' or string[x] != '2' or string[x] != '3' or string[x] != '4' or string[x] != '5' or string[x] != '6' or string[x] != '7' or string[x] != '8' or string[x] != '9' or string[x] != '.':
            string.replace(string[x], "")
    return string


def valid(string):
    # b'16:30:22,5.678\r\n'
    return True

# WE ALSO NEED A VALIDATION FUNCTION
def getTime(string):
    
    # b'16:30:22,5.678\r\n'
    # if it comes in as hours minutes seconds, AA:BB:CC
    if valid(string):

        #the start of the time string
        start = 0
        end = 0
    
        for x in range(0, len(string)): #I think the range func goes up by 1, so accessing up to length should work. Possibly length-1?
            if string[x] == "'":
                start = x + 1
            if string[x] == ",":
                end = x
                break

        return string[start:end]
    else:
        return False


def getPressure(string):
    # b'16:30:22,5.678\r\n'

##    arr = string.split(',')
##    for i in range(0, len(arr)):
##        for x in range(0, len(arr[i])):
##            if arr[i][x] != '0' or arr[i][x] != '1' or arr[i][x] != '2' or arr[i][x] != '3' or arr[i][x] != '4' or arr[i][x] != '5' or arr[i][x] != '6' or arr[i][x] != '7' or arr[i][x] != '8' or arr[i][x] != '9' or string[x] != '.':
##                arr[i].replace(arr[i][x], "")
##        print(arr[i])
    return string.split(',') #returns an arra


## Worst case, this is a hard coded algorithm to find everything in between the commas
##    array = []
##    
##    if valid(string):
##
##        commaCount = 0
##        for x in range(0, len(string)):
##            # Just getting the comma count for now
##            if string[x] == ',':
##                commaCount += 1
##
##        copy = commaCount
##        for i in range(0, len(string)):
##            if string[i] == ',':
##                copy += 1
##            if string[i:i+1] != '\r': #because apparently '\' is a special character???
##                if copy % 2 != 0:
##                    startIndex = i
##                else if copy % 2 == 0:
##                    endIndex = i
##                    string s = string[startIndex:endIndex]
##                    array += [s]
            
            

                    #change address as needed (check Arduino IDE->tools for this)
ser = serial.Serial("/dev/cu.usbmodem141301", 9600)

voltage = []
time = []
firstPT = []
secondPT = []

i=0

# HOW TO GET TIMESTAMPS FROM PYSERIAL?
# Hardcode this during hot fire to ONLY DISPLAY first two input streams. Play the time axis thing by ear, since I do not know how serial time input looks.
while i<10:
    string = str(ser.readline())
    ct = datetime.datetime.now()
    #print("current time = ", ct)
    ts = ct.timestamp()
    getPressure(string)
    i  += 1
    #print("timestamp:-", ts)
    arrayPT = getPressure(string) 
    time += [getTime(string)] #add 1 element
    firstPT += [arrayPT[0]] #add 1 element
    secondPT += [arrayPT[0]]                
    plt.plot(time, firstPT, linewidth = 1,
        marker='o', markerfacecolor='black', markersize=2)
    plt.plot(time, secondPT, linewidth = 1,
        marker='o', markerfacecolor='black', markersize=2)
    plt.pause(0.05)
    plt.show()
##    f= open("controlsDAQ.txt") # --> was messing around with ways to save the data
##    f.write("This is line %d\r\n")

## CURRENTLY:
##    - We can show two distinct lines on the same plot.
##    - I haven't had the time to code the ability for the program to produce as many lines as there are PT inputs
##            so for the hot fire test we'll have to hard code the arrays and plots. All values stored in the arrayPT array.
##    - also to do: storing data (see arduino code for this): https://www.guru99.com/reading-and-writing-files-in-python.html 
##    - also need to clean up lastmost element in array (i.e delete everything that isn't a number or period)
##    - need to write the data validation

