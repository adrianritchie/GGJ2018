import logging
import random
import time
import requests
from threading import Thread
from websocket_server import WebsocketServer

current_milli_time = lambda: int(round(time.time() * 1000))

def bomb_in_progress(time_limit):
	global running
	running = True
	print('running thread')
	end_time = current_milli_time() + (time_limit *1000)
	random.seed(end_time)
	server.send_message_to_all('c:s1:1')
	server.send_message_to_all('c:t1:0')
	server.send_message_to_all('c:t2:0')
	server.send_message_to_all('c:k1:0')
	server.send_message_to_all('x:0')
	while 1 == 1:
		remaining = end_time - current_milli_time()
		if remaining < 0:
			remaining = 0

		if random.randint(0, 100) > 95:
			server.send_message_to_all('c:t1:'+str(random.randint(0,2)))

		# if random.randint(0, 10) > 9:
		# 	server.send_message_to_all('c:k1:'+str(random.randint(0,9)))

		if random.randint(0, 100) > 95:
			server.send_message_to_all('c:s1:'+str(random.randint(0,1)))

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

payload = ["p","0","1","localhost:8080"]
requests.put("http://blynk-cloud.com/091b2133b443485dbb2e71686f361c6d/update/V0", json=payload )

server = WebsocketServer(8080, host='127.0.0.1', loglevel=logging.INFO)
server.set_fn_new_client(new_client)
server.set_fn_message_received(new_message_received)
server.run_forever()
