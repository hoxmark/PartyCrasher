import os, os.path

# simple version for working with CWD
# print len([name for name in os.listdir('.') if os.path.isfile(name)])

# path joining version for other paths
DIR = '/home/bhoxmark/PartyCrasher/counterexamples'
# DIR = '/Users/hoxmark/Developer/Fag/Cloud/PartyCrasher/counterexamples'

numberOfFiles = len([name for name in os.listdir(DIR) if os.path.isfile(os.path.join(DIR, name))])


file = open("count.txt", "r")
oldCount = file.read()

print(numberOfFiles)
print(oldCount)


if (int(oldCount) < int(numberOfFiles)):
    with open('count.txt', 'w') as file:
        print("Printing")
        file.write(str(numberOfFiles))
    


