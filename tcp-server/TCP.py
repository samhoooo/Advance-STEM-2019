import socket
import threading
import time
import queue
line_width = 5 # preferably odd number
window_size = 100

q = queue.Queue()
from graphics import *
from tkinter import *
master = Tk()
w = Canvas(master, width=line_width*window_size, height=line_width*window_size)
w.pack()

global x
global y
x = line_width*window_size/2
y = line_width*window_size/2
def put_draw(dx,dy):
	q.put(tuple((dx,dy)))
def update_point(dx,dy): # i meant line, oops
	global x
	global y
	w.create_line(x,y,x+line_width*dx, y+line_width*dy,fill="black", width=line_width)
	#line = Line(Point(x,y), Point(x+line_width*dx, y+line_width*dy))
	#line.setWidth(line_width)
	#line.draw(win)
	print('X: {}, Y: {}, NewX: {}, NewY: {}'.format(x,y,x+line_width*dx, y+line_width*dy))
	#x = x+line_width*dx
	#y = y+line_width*dy
	#Point(x,y).draw(win)

#win = GraphWin("Plot",line_width*window_size,line_width*window_size)
#win.setCoords(-win.getWidth()/2,-win.getHeight()/2,win.getWidth()/2,win.getHeight()/2)
# -window_size/2 to +window_size/2 for x and y

update_point(25,0)

bind_ip = '0.0.0.0'
bind_port = 9999

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(100)

print('Listening on {}:{}'.format(bind_ip, bind_port))


def handle_client_connection(client_socket):
	request = client_socket.recv(1024)
	hx,hy = tuple(request.decode("utf-8").split("#"))
	print('Received DX: {}, DY: {}'.format(hx,hy))
	put_draw(int(hx),int(hy))
	client_socket.close()

def loop_a():
	while True:
		client_sock, address = server.accept()
		print('Accepted connection from {}:{}'.format(address[0], address[1]))
		client_handler = threading.Thread(
			target=handle_client_connection,
			args=(client_sock,)
		)
		client_handler.start()
def loop_b():
	while True:
		dx,dy=q.get()
		update_point(dx,dy)
def loop_c():
	update()
p1 = threading.Thread(target=loop_a)
p2 = threading.Thread(target=loop_b)


p1.start()
p2.start()
mainloop()
update_point(0,5)
