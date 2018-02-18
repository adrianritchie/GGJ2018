import logging
import random
import time
from threading import Thread
from websocket_server import WebsocketServer

current_milli_time = lambda: int(round(time.time() * 1000))

def bomb_in_progress(time_limit):
	global running
	running = True
	print('running thread')
	end_time = current_milli_time() + (time_limit *1000)
	random.seed(end_time)
	server.send_message_to_all('c:s1:on')
	server.send_message_to_all('c:t1:0')
	server.send_message_to_all('c:t2:0')
	server.send_message_to_all('c:k1:0')
	server.send_message_to_all('x:0')
	while 1 == 1:
		remaining = end_time - current_milli_time()
		if remaining < 0:
			remaining = 0
		
		if 7000 <= remaining <= 7050:
			server.send_message_to_all('c:t1:1')

		if 6000 <= remaining <= 6050:
			server.send_message_to_all('c:t1:1')

		if 5000 <= remaining <= 5100:
			server.send_message_to_all('c:s1:off')

		server.send_message_to_all('r:'+str(remaining))

		if remaining == 0:
			break

		if not running: 
			break

		time.sleep(random.random()/10)

def new_client(client, server):
	print("New client connected.")		
	server.send_message_to_all("Hey, a new client has connected")

def new_message_received(client, server, message):
	global running, t
	print(message)
	if message[:1] == 'f' and running:
		running = False
		if message[2:] == '1':
			print('win')
		else:
			print('lose')

	if message[:1] == 't':
		running = False
		while t.isAlive():
			time.sleep(0.01)
			print('waiting for thread')
		t = Thread(target=bomb_in_progress, args=(int(message[1:]),))
		t.start()

running = False
t = Thread(target=bomb_in_progress, args=(0,))


server = WebsocketServer(8080, host='127.0.0.1', loglevel=logging.INFO)
server.set_fn_new_client(new_client)
server.set_fn_message_received(new_message_received)
server.run_forever()
