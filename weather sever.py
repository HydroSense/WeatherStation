#Terence McHugh 2016
#Hydrosense project
import serial
import time
import urllib
import urllib2

ser = serial.Serial('/dev/ttyUSB0',38400,timeout=.5)
base_url   = "http://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?ID=KPALEWIS25&PASSWORD=uy1yg0ke&dateutc=now&"
filename="/var/tmp/wxdata/data.txt"

while True:         #infinate loop, server always checking for a post
    file= open(filename,'a') #opens data.txt
    while True:# while data from weather station hasn't come yet
        incoming = ser.readline().strip()  # get line from xbee
        if  incoming!= b'':   # if data was ent
            part1="winddir="
            count=1
            data=""
            for x in incoming:  #handels parsing the numbers seperated by +
                                #puts tags for weewx before each one
                if(x=="+" and count == 1):
                    part1+="&windspeedmph="
                    count=2
                    file.write("winddir="+data+'\n')
                    data=""
                elif(x=="+" and count == 2):
                    part1+="&rainin="
                    file.write("windspeedmph="+data+'\n')
                    data=""
                else:
                    part1+=x
                    data+=x
            
            file.write("rainin="+data+'\n')
            print(part1)
            break  # done with weather board data
    part1+="&"
    while True: # temp and humidity
        incoming = ser.readline().strip()
        if  incoming!= b'':
            print(incoming)
            part2=incoming #already formatted
            break
    print(base_url+part1+part2)
    urllib2.urlopen(base_url+part1+part2, None) # make get comand to weather underground
    lines=part2.split("&") # format for weewx file parser
    file.write(lines[0]+'\n')
    file.write(lines[1]+'\n')
    file.close()

