import subprocess
import time

best = 62
while True:

    cmd = "/Users/hoxmark/Developer/Fag/Cloud/PartyCrasher/a.out"
    arg = "counterexamples/"+str(best)+".txt"
    proc = subprocess.Popen(
                "/Users/hoxmark/Developer/Fag/Cloud/PartyCrasher/a.out counterexamples/163.txt",
                shell=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                universal_newlines=True,
                bufsize=1        
            )


    # while proc.poll() is None:
    #     output = proc.stdout.readline()
    #     print(output)

    output = ''
    while True:
        result = proc.poll()
        delta = proc.stdout.read(1)
        # print("++")
        if result is not None:
            print(result)
            print(best)
            best += 1;
            break