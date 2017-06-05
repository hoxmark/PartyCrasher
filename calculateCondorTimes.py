import re

days = 0
hours = 0
minutes = 0
seconds = 0

total_runtime_hours = 0

fileName = "condor_runtimes.txt"
with open(fileName) as f:
    for line in f:
        line = line.strip().replace('+', ':')
        values = line.split(':')
        days += int(values[0])
        hours += int(values[1])
        minutes += int(values[2])
        seconds += int(values[3])

total_runtime_hours += days * 24
total_runtime_hours += hours
total_runtime_hours += minutes / 60
total_runtime_hours += seconds / 3600
print(total_runtime_hours)
