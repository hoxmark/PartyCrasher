import subprocess
import time

cmd = 'ping 8.8.8.8'

proc = subprocess.Popen(
            cmd,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

output = ''
while True:
    result = proc.poll()
    delta = proc.stdout.read(1)

    if result is not None:
        print 'terminated'
        break

    if delta != ' ':
        output = output + delta
    else:
        if '%' in output:
            print 'percentage is:'
            print output

        elif '/s' in output:
            print 'speed is:'
            print output

        print 'output is:'
        print output
        output = ''