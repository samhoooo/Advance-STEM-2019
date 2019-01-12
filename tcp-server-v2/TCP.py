import socket
import threading
import queue
import array
from colorama import *
init(autoreset=True)
map = [[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,1,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0]]
x = 4
y = 4

xWASD = {
        "A": -1,
        "D": 1
}
	
yWASD = {
        "W": -1,
        "S": 1
    }


bind_ip = '0.0.0.0'
bind_port = 9999

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(100)

print('Listening on {}:{}'.format(bind_ip, bind_port))


def handle_client_connection(client_socket):
	global x
	global y
	global xWASD
	global yWASD
	request = client_socket.recv(1024)
	wasd,val1,val2,val3,val4 = tuple(request.decode("utf-8").rstrip("\n\r").split("#"))
	print('Received WASD: {}, Values: {}, {}, {}, {}'.format(wasd,val1,val2,val3,val4))
	
	
	# updating map
	x += xWASD.get(wasd,0)
	y += yWASD.get(wasd,0)
	if y < 1:
		map.insert(0,[0 for x in range(len(map[0]))])
		y += 1
	if (y+2) > len(map):
		map.append([0 for x in range(len(map[0]))])
	if x < 1:
		for r in map:
			r.insert(0,0)
		x += 1
	if (x+2) > len(map[0]):
		for r in map:
			r.append(0)
	if map[y+1][x] != 99:
		map[y+1][x]=val1
	if map[y][x+1] != 99:
		map[y][x+1]=val2
	if map[y-1][x] != 99:
		map[y-1][x]=val3
	if map[y][x-1] != 99:
		map[y][x-1]=val4
	map[y][x]=99
	# print map
	for ir in range(len(map)):
		for ic in range(len(map[ir])):
			if ir==y and ic==x:
				print(Style.BRIGHT + Fore.BLUE + "██" ,end="")
			elif map[ir][ic] == 99:
				print(Fore.GREEN + str(map[ir][ic]).zfill(2) ,end="")
			elif map[ir][ic] != 0:
				print(Fore.YELLOW + str(map[ir][ic]).zfill(2) ,end="")
			else:
				print(Fore.RED + str(map[ir][ic]).zfill(2) ,end="")
		print()
	
	
	print('New coord: {},{}'.format(x,y))
	
	client_socket.close()

def loop_a():
	while True:
		client_sock, address = server.accept()
		print('Accepted connection from {}:{}'.format(address[0], address[1]))
		client_handler = threading.Thread(target=handle_client_connection,args=(client_sock,))
		client_handler.start()

	
p1 = threading.Thread(target=loop_a)



#p1.start()



while True: 
	loop_a()

