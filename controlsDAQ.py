import matplotlib.pyplot as plt
import serial
from serial import Serial
import csv
import time
import datetime

def valid(string):
    # b'16:30:22,5.678\r\n'
    return True
 
def getTime(string):
    
    # b'16:30:22,5.678\r\n'
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

    array = string.split(',') #returns an array

    #Clean up elements of array
    for i in (0, len(array)-1):
        res = array[i]
        for x in range(0, len(res)):
            if res[x] != '0' and res[x] != '1' and res[x] != '2' and res[x] != '3' and res[x] != '4' and res[x] != '5' and res[x] != '6' and res[x] != '7' and res[x] != '8' and res[x] != '9' and res[x] != '.':
                res.replace(res[x], "")

    return array
    #for x in (0,len(array))


                    #change address as needed (check Arduino IDE->tools for this)
ser = serial.Serial("/dev/cu.usbmodem141301", 9600)

voltage = []
time = []
firstPT = []
secondPT = []
analogTime = [] #this is the time that I would rather not use; independent from arduino
testAxis = []

i=0

pt1 = open("pt1.txt","w+")
pt2 = open("pt2.txt","w+")

itera = 0

# HOW TO GET TIMESTAMPS FROM PYSERIAL?
# Hardcode this during hot fire to ONLY DISPLAY first two input streams. Play the time axis by ear, since I do not know how serial time input looks.
while itera < 1000:
    string = str(ser.readline()) # reading in serial input
    ct = datetime.datetime.now() #current time
    analogTime += [str(ct)[11:19]] # current x axis, if we can't get time from the serial input

    ## debugging
    i  += 1
    testAxis += [i]

    ## pressure transducer array
    arrayPT = getPressure(string)

    pt1string = arrayPT[0] 
    pt2string = arrayPT[0] 

    length = len(pt1string)

    new = ""
    numCount = 0
    endIndex = 0
    startIndex = 0

    for x in range(0,length):
        if pt1string[x] == '0' or pt1string[x] == '1' or pt1string[x] == '2' or pt1string[x] == '3' or pt1string[x] == '4' or pt1string[x] == '5' or pt1string[x] == '6' or pt1string[x] == '7' or pt1string[x] == '8' or pt1string[x] == '9' or pt1string[x] == '.' or pt1string[x] == ':':
            numCount += 1
            if numCount == 1:
                startIndex = x
                
    endIndex = numCount

    pt1string = pt1string[startIndex:endIndex + 2]

    length2 = len(pt2string)
    new2 = ""
    numCount2 = 0
    endIndex2 = 0
    startIndex2 = 0

    for x in range(0,length2):
        if pt2string[x] == '0' or pt2string[x] == '1' or pt2string[x] == '2' or pt2string[x] == '3' or pt2string[x] == '4' or pt2string[x] == '5' or pt2string[x] == '6' or pt2string[x] == '7' or pt2string[x] == '8' or pt2string[x] == '9' or pt2string[x] == '.' or pt2string[x] == ':':
            numCount2 += 1
            if numCount2 == 1:
                startIndex2 = x
                
    endIndex2 = numCount2

    pt2string = pt2string[startIndex2:endIndex2 + 2]

    ## hardcoding the plots, based on how many data streams we have coming in for pressure
    firstPT += [pt1string] #add 1 element
    secondPT += [pt2string]                

    plt.pause(0.05)

    # Y AXIS
    print(pt1string)
    print(pt2string)

    # X AXIS - YES!!!!! JUST PRINTING NUMS NOW
    print(str(ct)[11:19])

    #test plot
    plt.plot(analogTime, firstPT, linewidth = 1, marker='o', markerFaceColor ='black', markersize=2)
    
    for i in range(10):
        pt1.write(str(ct)[11:19] + ', ' + pt1string + '; ')
        pt2.write(str(ct)[11:19] + ', ' + pt2string + '; ')

    itera += 1


## After while loop
pt1.close()
pt1=open("pt1.txt", "r")
pt1.read()
pt2.close()
pt2=open("pt1.txt", "r")
pt2.read()
plt.show()
