import socket
import threading
import queue
from colorama import *
import os

def cls():
	os.system('cls' if os.name=='nt' else 'clear')

q = queue.Queue()

init(autoreset=True)
map = [[1,1,1,1,1],[1,1,1,1,1],[1,1,99,1,1],[1,1,1,1,1],[1,1,1,1,1]]
x = y = 2

xWASD = {
	"A": -1,
	"D": 1
}
	
yWASD = {
	"W": -1,
	"S": 1
}
numCHAR = {
	0:Back.RED+Fore.BLACK + "XX",
	99:Back.GREEN + "  ",
	1:Back.BLUE+Fore.BLACK + "[]"

}



bind_ip = '0.0.0.0'
bind_port = 9999

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(1)

print('Listening on {}:{}'.format(bind_ip, bind_port))


def handle_client_connection(client_socket,ip,name):
	q.put((*(client_socket.recv(1024).decode("utf-8").rstrip("\n\r").split("#")),ip,name))
	client_socket.close()

def taskA():
	while True: 
		client_sock, address = server.accept()
		threading.Thread(target=handle_client_connection,args=(client_sock,address[0], address[1],)).start()
		#client_handler
p1 = threading.Thread(target=taskA)
#p2 = threading.Thread(target=taskB, args=(*args, **kwargs))

p1.start()
#p2.start()
while True:
	if not q.empty():
		cls()
		wasd,valW,valA,valS,valD,ip,addr = q.get()
		valW = [ int(x) for x in valW.split("&")]
		valA = [ int(x) for x in valA.split("&")]
		valS = [ int(x) for x in valS.split("&")]
		valD = [ int(x) for x in valD.split("&")]
		x += xWASD.get(wasd,0)
		y += yWASD.get(wasd,0)
		print('{}:{}: Going: {}>({},{})\nValues:\n{}\n{}\n{}\n{}'.format(ip,addr,wasd,x,y,valW,valA,valS,valD))
					
		if (y-len(valW)) < 0:
			map = [[1 for xxxx in range(len(map[0]))] for xxxxx in range(-(y-len(valW)))]+map
			y += -(y-len(valW))
		if (y+len(valS)) > len(map)-1:
			map = map+[[1 for xxxx in range(len(map[0]))] for xxxxx in range((y+len(valS))-len(map)+1)]
		if (x+len(valD)) > len(map[0])-1:
			add = [1 for xxxx in range((x+len(valD))-len(map[0])+1)]
			for rowid in range(len(map)):
				map[rowid] = map[rowid] + add
		if (x-len(valA)) < 0:
			add = [1 for xxxx in range(-(x-len(valA)))]
			for rowid in range(len(map)):
				map[rowid] = add + map[rowid]
			x += -(x-len(valA))
		for ggg in range(len(valW)):
			if map[y-(ggg+1)][x] != 99:
				map[y-(ggg+1)][x]=valW[ggg]
		for ggg in range(len(valA)):
			if map[y][x-(ggg+1)] != 99:
				map[y][x-(ggg+1)]=valA[ggg]
		for ggg in range(len(valS)):
			if map[y+(ggg+1)][x] != 99:
				map[y+(ggg+1)][x]=valS[ggg]
		for ggg in range(len(valD)):
			if map[y][x+(ggg+1)] != 99:
				map[y][x+(ggg+1)]=valD[ggg]
		map[y][x]=99

		if not q.qsize():
			for ir in range(len(map)):
				pstr = ''
				for ic in range(len(map[ir])):
					num = int(map[ir][ic])
					if ir==y and ic==x:
						pstr += Back.CYAN + "  "
					else:
						pstr += numCHAR.get(num,Back.YELLOW+Fore.WHITE+str(num).zfill(2))
				print(pstr)