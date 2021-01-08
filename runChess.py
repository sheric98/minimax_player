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
    # p = subprocess.Popen(
    #     ['./playChess', '0', '0', '1', '-a', '4', '-q'],
    #     stdout=subprocess.PIPE,
    #     stdin=subprocess.PIPE,
    #     text=True,
    #     bufsize=1,
    #     universal_newlines=True
    # )
    # q = Queue()
    # l = Lock()
    # t = Thread(target=enqueue_output, args=(p.stdout, q, l))
    # t.daemon = True
    # t.start()
    d, game = interact.init_game()
    interact.make_move('d4', game, d)
    # cpu_turn = True
    
    # while True:
    #     try:
    #         line = q.get(timeout=.1)
    #         if line == 'finished':
    #             break
    #         with l:
    #             if t.is_alive() and cpu_turn:
    #                 print('got move ' + line)
    #                 interact.make_move(line, game, d)
    #                 print("made move " + line)
    #     except Empty:
    #         if not cpu_turn:
    #             move_str = interact.get_most_recent_move(game, d)
    #             print("website made move " + move_str)
    #             p.stdin.write(move_str + '\n')
    #         else:
    #             continue

