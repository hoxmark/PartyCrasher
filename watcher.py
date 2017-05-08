#!/usr/bin/env python
import os, os.path
import smtplib

# simple version for working with CWD
# print len([name for name in os.listdir('.') if os.path.isfile(name)])

fromaddr = 'partycrasherucsb@gmail.com'
toaddrs  = 'hoxmark@me.com'
username = 'partycrasherucsb@gmail.com'
password = 'ucsb1234'


# path joining version for other paths
DIR = '/home/bhoxmark/PartyCrasher/counterexamples'
# DIR = '/Users/hoxmark/Developer/Fag/Cloud/PartyCrasher/counterexamples'

numberOfFiles = len([name for name in os.listdir(DIR) if os.path.isfile(os.path.join(DIR, name))])


file = open("/home/bhoxmark/PartyCrasher/count.txt", "r")
oldCount = file.read()

print(numberOfFiles)
print(oldCount)
valu = int(numberOfFiles)+72
msg = 'New value found: '+str(valu)


if (int(oldCount) < int(numberOfFiles)):
    with open('/home/bhoxmark/PartyCrasher/count.txt', 'w') as file:
        print("Printing")
        file.write(str(numberOfFiles))
        server = smtplib.SMTP('smtp.gmail.com:587')
        server.starttls()
        server.login(username,password)
        server.sendmail(fromaddr, toaddrs, msg)
        server.quit()
    


