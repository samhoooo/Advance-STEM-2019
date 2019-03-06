#!/usr/bin/env python3
import socket
import threading
import queue
from colorama import *
import os
import time
import sys




cs = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
cs.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
cs.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)


def cls():
	os.system('cls' if os.name=='nt' else 'clear')

q = queue.Queue()

init(autoreset=True)
map = [[1,1,1,1,1],[1,1,1,1,1],[1,1,1,1,1],[1,1,1,1,1],[1,1,1,1,1]]
xID = {}
yID = {}
spset = False
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
server.settimeout(100)
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
def taskB():
	while True:
		cs.sendto(b'!', ('255.255.255.255', 9876))
		time.sleep(1)
threading.Thread(target=taskA).start()
threading.Thread(target=taskB).start()

while True:
	if not q.empty():
		cls()
		id,wasd,valW,valA,valS,valD,ip,addr = q.get()
		if (valW and valA and valS and valD):
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
		if not id in xID:
			xID[id] = ic
		if not id in yID:
			yID[id] = ir
		xID[id] += xWASD.get(wasd,0)
		yID[id] += yWASD.get(wasd,0)
		print('ID:{}@{}:{}: Going: {}>({},{})\nValues:\n{}\n{}\n{}\n{}'.format(id,ip,addr,wasd,xID[id],yID[id],valW,valA,valS,valD))

		if (yID[id]-len(valW)) < 0:
			map = [[1 for i in range(len(map[0]))] for j in range(-(yID[id]-len(valW)))]+map
			yID[id] -= yID[id]-len(valW)
		if (yID[id]+len(valS)) > len(map)-1:
			map += [[1 for i in range(len(map[0]))] for j in range((yID[id]+len(valS))-len(map)+1)]
		if (xID[id]+len(valD)) > len(map[0])-1:
			add = [1 for i in range((xID[id]+len(valD))-len(map[0])+1)]
			for rowid in range(len(map)):
				map[rowid] += add
		if (xID[id]-len(valA)) < 0:
			add = [1 for i in range(-(xID[id]-len(valA)))]
			for rowid in range(len(map)):
				map[rowid] = add + map[rowid]
			xID[id] = xID[id]-(xID[id]-len(valA))
		if not spset:
			map[yID[id]][xID[id]]=99
			spset=True
		else:
			if not map[yID[id]][xID[id]] == 99:
				map[yID[id]][xID[id]]=98

		for i in range(len(valW)):
			if map[yID[id]-(i+1)][xID[id]] < 98:
				map[yID[id]-(i+1)][xID[id]]=valW[i]
		for i in range(len(valA)):
			if map[yID[id]][xID[id]-(i+1)] < 98:
				map[yID[id]][xID[id]-(i+1)]=valA[i]
		for i in range(len(valS)):
			if map[yID[id]+(i+1)][xID[id]] < 98:
				map[yID[id]+(i+1)][xID[id]]=valS[i]
		for i in range(len(valD)):
			if map[yID[id]][xID[id]+(i+1)] < 98:
				map[yID[id]][xID[id]+(i+1)]=valD[i]
		

		if not q.qsize():
			for ir in range(len(map)):
				pstr = ''
				for ic in range(len(map[ir])):
					num = int(map[ir][ic])
					if ir==yID[id] and ic==xID[id]:
						pstr += Back.CYAN + "  "
					else:
						pstr += numCHAR.get(num,Back.YELLOW+Fore.WHITE+str(num).zfill(2))
				print(pstr)
