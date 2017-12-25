#!/usr/bin/python
# coding: utf-8
import os
import socket
import sys
import select

def init_socket(clients, num):
	if num < 0:
		pass
	for n in xrange(0, num):
		try:
			fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
		except socket.error, e:
			print e
			break	
		clients.append(fd)

class MyClient():
	def __init__(self, clients, num, host):
		self.num = num
		self.clients = clients
		self.host = host
	def run(self):
		clis = self.clients
		nclis = self.num
		h = self.host
		init_socket(clis, nclis)
		for c in clis:
			try:
				c.connect(h)
			except socket.error, e:
				print e
				return
		wf = clis[:]
		ef = clis[:]
		rf = clis
		rf.append(sys.stdin)
		while True:
			rn, wn, en = select.select(rf, [], ef)
			for cl in rn:
				if cl == sys.stdin:
					mm = sys.stdin.readline()
					mm = mm[:-1]
					if mm:
						for c in wf:
							c.send(mm)
				else:
					msg = cl.recv(1024)
					if msg:
						if msg == 'HEARTBEAT':
							cl.send('HEARTBEAT')
						else:
							print "Recved : %s" %msg
					else:
						cl.close()
						rf.remove(cl)
						ef.remove(cl)
			for ecl in en:
				print 'RST ?'
				ecl.close()
				rf.remove(ecl)
				ef.remove(ecl)

if __name__ == '__main__':
	clients = []
	num = 1
	host = ('192.168.179.128', 6666)
	mc = MyClient(clients, num, host)
	mc.run()
	
