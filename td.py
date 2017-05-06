import sys
from subprocess import PIPE, Popen
from threading  import Thread

try:
    from Queue import Queue, Empty
except ImportError:
    from queue import Queue, Empty  # python 3.x

ON_POSIX = 'posix' in sys.builtin_module_names

def enqueue_output(out, queue):
    for line in iter(out.readline, b''):
        queue.put(line)
    out.close()

cmd = "/Users/hoxmark/Developer/Fag/Cloud/PartyCrasher/a.out"
arg = "counterexamples/63.txt"
p = Popen([cmd, arg], stdout=PIPE, bufsize=1, close_fds=ON_POSIX)
q = Queue()
t = Thread(target=enqueue_output, args=(p.stdout, q))
t.daemon = True # thread dies with the program
t.start()

# ... do other things here

# read line without blocking

while(True):
    try:  line = q.get(timeout=1)
    except Empty:
        print('no output yet')
    else: # got line
        # ... do something with line
        print(line)