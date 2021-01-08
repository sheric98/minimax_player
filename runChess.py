import subprocess
import sys
from threading import Thread, Lock
import time
from queue import Queue, Empty
from chessWeb import interact

def enqueue_output(out, queue, lock):
    for line in iter(out.readline, b''):
        stripped = line.rstrip('\n')
        queue.put(stripped)
        if stripped == "finished":
            break
    with lock:
        out.close()

def send_pipe(p, inp):
    p.communicate(inp)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Must pass in one argument as depth of minimax player')
        exit(-1)

    try:
        depth = int(sys.argv[1])
    except ValueError:
        print("depth must be an integer")
        exit(-1)

    p = subprocess.Popen(
        ['./playChess', '0', '0', '1', '-a', str(depth), '-q'],
        stdout=subprocess.PIPE,
        stdin=subprocess.PIPE,
        text=True,
        bufsize=1,
        universal_newlines=True
    )
    q = Queue()
    l = Lock()
    t = Thread(target=enqueue_output, args=(p.stdout, q, l))
    t.daemon = True
    t.start()
    d, game = interact.init_game()
    cpu_turn = True
    
    while True:
        try:
            line = q.get(timeout=.1)
            if line == 'finished':
                break
            with l:
                if t.is_alive() and cpu_turn:
                    interact.make_move(line, game, d)
                    print("made move " + line)
                    cpu_turn = False
        except Empty:
            if not cpu_turn:
                move_str = interact.get_most_recent_move(game, d)
                print("website made move " + move_str)
                cpu_turn = True
                p.stdin.write(move_str + '\n')
            else:
                continue

    d.close()
