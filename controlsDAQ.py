import matplotlib.pyplot as plt
import serial
from serial import Serial

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

def valid(string):
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


def getPressure(string):
    # b'16:30:22,5.678\r\n'

    return string.split(',') #returns an array


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
totalPT = []
i = 0;

while i<2:
    string = str(ser.readline())
    time += [getTime(string)]
    arrayPT = getPressure(string)
    totalPT += arrayPT

    # PRODUCE SEPARATE PLOTS

    for i in range(0,len(arrayPT)):
        if i > 1 & i < len(arrayPT) - 1:
            place = arrayPT[i]
            plt.plot(time, place, linewidth = 1,
         marker='o', markerfacecolor='black', markersize=2)

    plt.show()
    plt.pause(0.05)
            # plot array[i] vs t in plot that it corresponds to

    i += 1
    
##    plt.plot(time, totalPT, linewidth = 1,
##         marker='o', markerfacecolor='black', markersize=2)
##    plt.pause(0.05)


