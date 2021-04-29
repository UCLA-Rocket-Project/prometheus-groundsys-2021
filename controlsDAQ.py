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

# WE ALSO NEED A VALIDATION FUNCTION
def getTime(string):
    
    # b'16:30:22,5.678\r\n'
    # if it comes in as hours minutes seconds, AA:BB:CC
    if valid(string):

        firstComPlace = 0;
            # is it range(0, string.length()?)
        for x in string.length():
            if string[x] == ',':
                firstComPlace = x;
                break
            else:
                return False
        
        for i in string.length():
            if string[i] == "'":
                for j in string.length():
                    if string[j] == string[firstComPlace]:
                        time = string[i:j]
                        return validString
                    else:
                        return False #BUT we shouldn't need to return false since
                                     # valid(string) should check for that
            else:
                return False
    else:
        return False
        

def getPressure(string, n):
    # b'16:30:22,5.678\r\n'
    if valid(string):
# FIX LATER: WE DON'T WANT TO ACCESS LIKE ARRAY. WE WANT TO GET WHATEVER IS
#PAST b' AND BEFORE COMMA # 1
         return string[12:16]

def valid(string):
    return True

                    #change address as needed (check Arduino IDE->tools for this)
ser = serial.Serial("/dev/cu.usbmodem144101", 9600)

timeInc = 1
voltage = []
time = []
pressure = []
i = 0;

# garbage input that comes at first
# , [THING] \ = end of string
# , [THING , = middle parameter IF THERE IS ONE (probably won't be)
# ' [THING] , = start of string --> time stamp

# ONLY GRAPH VALID INPUT
while i<6:
    cc=str(ser.readline())
    pressure += [cc[8:13]]
    voltage += [cc[2:10]]
    #print("x = ", cc[2:7])
    #print("y = ", cc[8:13])
    getTime(cc)
    print(cc[2:10])
    #print(cc)
    i+=1
    plt.plot(voltage, pressure, linewidth = 1,
         marker='o', markerfacecolor='black', markersize=2)
    plt.pause(0.05)

plt.show()

# The code below creates a stagnant plot that only shows up after the arduino is done collecting data.

## voltage = []
## time = []
## pressure = []
## i = 0;
##ser = serial.Serial("/dev/cu.usbmodem141301", 9600)
##while (i < 5):
##     cc=str(ser.readline())
##     voltage += [cc[2:][:-5]]
##     pressure += [1]
##     print("I am working")
##     plt.plot(voltage, pressure, linewidth = 3,
##         marker='o', markerfacecolor='black', markersize=8)
##     i += 1
##    # plt.show()
##     
##   #  print(cc[2:][:-5])
##
###plt.plot(voltage, pressure, linewidth = 3,
##        # marker='o', markerfacecolor='black', markersize=8)
##  
### X axis
##plt.xlabel('Voltage')
##
### Y axis
##plt.ylabel('Pressure')
##  
##plt.title('Voltage v. Pressure test from ARES PT')
##  
##plt.show()
