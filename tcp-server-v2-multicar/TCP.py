import socket
import threading
import queue
from colorama import *
import os

def cls():
	os.system('cls' if os.name=='nt' else 'clear')

q = queue.Queue()

init(autoreset=True)
map = [[1,1,1,1,1],[1,1,1,1,1],[1,1,1,1,1],[1,1,1,1,1],[1,1,1,1,1]]
xid = {}
yid = {}
spset=False
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
	98:Back.GREEN + "  ",
	1:Back.BLUE+Fore.BLACK + "[]",
	99:Back.GREEN + Fore.WHITE + "##"

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
		id,wasd,valW,valA,valS,valD,ip,addr = q.get()
		valW = [ int(x) for x in valW.split("&")]
		valA = [ int(x) for x in valA.split("&")]
		valS = [ int(x) for x in valS.split("&")]
		valD = [ int(x) for x in valD.split("&")]
		do_loop = True
		ic = 0
		ir = 0
		for ir in range(len(map)):
			for ic in range(len(map[ir])):
				if int(map[ir][ic]) == 99:
					do_loop = False
					break
			if not do_loop:
				break
		print(ic)
		print(ir)
		if not id in xid:
			xid[id] = ic
		if not id in yid:
			yid[id] = ir
		xid[id] += xWASD.get(wasd,0)
		yid[id] += yWASD.get(wasd,0)
		print('ID:{}@{}:{}: Going: {}>({},{})\nValues:\n{}\n{}\n{}\n{}'.format(id,ip,addr,wasd,xid[id],yid[id],valW,valA,valS,valD))
		if (yid[id]-len(valW)) < 0:
			map = [[1 for xxxx in range(len(map[0]))] for xxxxx in range(-(yid[id]-len(valW)))]+map
			yid[id] -= yid[id]-len(valW)
		if (yid[id]+len(valS)) > len(map)-1:
			map += [[1 for xxxx in range(len(map[0]))] for xxxxx in range((yid[id]+len(valS))-len(map)+1)]
		if (xid[id]+len(valD)) > len(map[0])-1:
			add = [1 for xxxx in range((xid[id]+len(valD))-len(map[0])+1)]
			for rowid in range(len(map)):
				map[rowid] += add
		if (xid[id]-len(valA)) < 0:
			add = [1 for xxxx in range(-(xid[id]-len(valA)))]
			for rowid in range(len(map)):
				map[rowid] = add + map[rowid]
			xid[id] = xid[id]-(xid[id]-len(valA))
		if not spset:
			map[yid[id]][xid[id]]=99
			spset=True
		else:
			map[yid[id]][xid[id]]=98
		for ggg in range(len(valW)):
			if map[yid[id]-(ggg+1)][xid[id]] < 98:
				map[yid[id]-(ggg+1)][xid[id]]=valW[ggg]
		for ggg in range(len(valA)):
			if map[yid[id]][xid[id]-(ggg+1)] < 98:
				map[yid[id]][xid[id]-(ggg+1)]=valA[ggg]
		for ggg in range(len(valS)):
			if map[yid[id]+(ggg+1)][xid[id]] < 98:
				map[yid[id]+(ggg+1)][xid[id]]=valS[ggg]
		for ggg in range(len(valD)):
			if map[yid[id]][xid[id]+(ggg+1)] < 98:
				map[yid[id]][xid[id]+(ggg+1)]=valD[ggg]
		

		if not q.qsize():
			for ir in range(len(map)):
				pstr = ''
				for ic in range(len(map[ir])):
					num = int(map[ir][ic])
					if ir==yid[id] and ic==xid[id]:
						pstr += Back.CYAN + "  "
					else:
						pstr += numCHAR.get(num,Back.YELLOW+Fore.WHITE+str(num).zfill(2))
				print(pstr)