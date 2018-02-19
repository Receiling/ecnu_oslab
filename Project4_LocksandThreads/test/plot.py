# encoding:utf-8
import matplotlib.pyplot as plt
import json, sys
from math import log10

lock_mp = {1:"SPINLOCK",2:"MUTEX",3:"TWOPHASE",4:"PTHREAD"}
import os

def clean(filename):
	with open(filename, "w") as f:  # clear this file
		f.write("")

def update_line(filename, before, after):  # replace line with before into after
	with open(filename, "r") as f:
		lines = f.readlines()
		for i in range(0,len(lines)):
			if before in lines[i]:
				lines[i] = after+"\n"
				break
	with open(filename, "w") as f:
		f.writelines(lines)		

def twophase_test():
	filename = "twophase_result.txt"
	clean(filename)
	update_line("lock.h", "#define LOCK_TYPE", "#define LOCK_TYPE 3")
	spin_times = 0
	while spin_times<=100:
		spin_times +=5 
		update_line("twophase.h","#define SPIN_TIMES","#define SPIN_TIMES %d" % (spin_times))
		os.system("make && ./timer twophase >> twophase_result.txt")
		
def plot_twophase():
	filename = "twophase_result.txt"
	lines = open(filename,"r").readlines()
	plt.figure()
	for i in range(0,len(lines)):
		if len(lines[i]) > 0:
			lines[i] = json.loads(lines[i][:-1])

	plt.xlabel("SPIN_TIMES");plt.ylabel("Average time (ms)")

	for thread_num in [1,5,9,13,17]:
		X = []
		Y = []
		for x in lines:
			if x["THREADS"] == thread_num:
				X.append(x["SPIN_TIMES"])
				Y.append(x["AVG"])
		plt.plot(X,Y,label = "%d Threads" % thread_num)
		plt.legend()
	plt.show()	

def list_test():
	filename = "list_result.txt"
	clean(filename)
	for lock_type in range(1,5):
		update_line("lock.h", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		update_line("lock.c", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		os.system("make &&./timer list>> list_result.txt")

def hash_test():
	filename = "hash_result.txt"
	clean(filename)
	#  test 1
	update_line("hash.h","#define HASH_FIX_THREADS","#define HASH_FIX_THREADS 0")
	for lock_type in range(1,5):
		update_line("lock.h", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		update_line("lock.c", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		os.system("make &&./timer hash>> hash_result.txt")
	#  test 2
	update_line("hash.h","#define HASH_FIX_THREADS","#define HASH_FIX_THREADS 1")
	for lock_type in range(1,5):
		update_line("lock.h", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		update_line("lock.c", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		os.system("make &&./timer hash>> hash_result.txt")		

def counter_test():
	filename="counter_result.txt"
	clean(filename)
	for lock_type in range(1,5):
		update_line("lock.h", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		update_line("lock.c", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		os.system("make &&./timer counter>> counter_result.txt")
		
def plot_list():
	filename = "list_result.txt"
	lines = open(filename,"r").readlines()
	plt.figure()
	for i in range(0,len(lines)):
		if len(lines[i]) > 0:
			lines[i] = json.loads(lines[i][:-1])

	for lock_type in range(1,5):
		plt.subplot(240 + lock_type)
		plt.xlabel("Thread Numbers");plt.ylabel("Average time (ms)")
		for test_type in range(1,5):
			X = []
			Y = []
			for x in lines:
				if x["LOCK"]==lock_mp[lock_type] and x["TEST_TYPE"] == test_type:
					X.append(x["THREADS"])
					Y.append(x["AVG"])
			plt.plot(X,Y,label = "TEST %d %s " % (test_type, lock_mp[lock_type]))
			plt.legend()
		
	for test_type in range(1,5):	
		plt.subplot(244 + test_type)
		plt.xlabel("Thread Numbers");plt.ylabel("Average time (ms)")
		for lock_type in range(1,5):
			X = []
			Y = []
			for x in lines:
				if x["LOCK"]==lock_mp[lock_type] and x["TEST_TYPE"] == test_type:
					X.append(x["THREADS"])
					Y.append(x["AVG"])
				
			plt.plot(X,Y,label = "TEST %d %s " % (test_type, lock_mp[lock_type]))
			plt.legend()

	plt.show()
	
def plot_hash():
	filename = "hash_result.txt"
	plt.figure()
	lines = open(filename,"r").readlines()
	for i in range(0,len(lines)):
		lines[i] = json.loads(lines[i][:-1])
	# 各种锁在不同线程数下花的时间
	plt.subplot(121)
	plt.xlabel("Thread Numbers");plt.ylabel("Average time (ms)")
	for lock_type in range(1,5):
		X = []
		Y = []
		for x in lines:
			if x["LOCK"]==lock_mp[lock_type] and x["TEST_TYPE"] == 1 and x["MAX_SIZE"] == int(1e6):
				X.append(x["THREADS"])
				Y.append(x["AVG"])
		print(X,Y)
		plt.plot(X,Y,label = "%s " % (lock_mp[lock_type]))
		plt.legend()	
	# -----------------------
	plt.subplot(122)
	plt.xlabel("MAX_SIZE");plt.ylabel("Average time (ms)")
	for lock_type in range(1,5):
		X = []
		Y = []
		#for max_size in [1, 10,100,1000,10000,100000,1000000]:
		for x in lines:
			if x["LOCK"]==lock_mp[lock_type] and x["TEST_TYPE"] == 2 and x["MAX_SIZE"] % 10 == 0:
				X.append(x["MAX_SIZE"])
				Y.append(x["AVG"])
		plt.plot(X,Y,label = "%s " % (lock_mp[lock_type]))
		plt.legend()
	plt.show()

def plot_counter():
	filename = "counter_result.txt"
	lines = open(filename,"r").readlines()
	plt.figure()
	for i in range(0,len(lines)):
		if len(lines[i]) > 0:
			lines[i] = json.loads(lines[i][:-1])

	for lock_type in range(1,5):
		plt.xlabel("Thread Numbers");plt.ylabel("Average time (ms)")
		X = []
		Y = []
		for x in lines:
			if x["LOCK"]==lock_mp[lock_type] :
				X.append(x["THREADS"])
				Y.append(x["AVG"])
		plt.plot(X,Y,label = "%s " % (lock_mp[lock_type]))
		plt.legend()

	plt.show()

def plot_fairness():
	filename = "fairness_result.txt"
	lines = open(filename,"r").readlines()
	plt.figure()
	for i in range(0,len(lines)):
		if len(lines[i]) > 0:
			lines[i] = json.loads(lines[i][:-1])

	for lock_type in range(1,5):
		plt.xlabel("Thread Numbers");plt.ylabel("Fairness")
		X = []
		Y = []
		for x in lines:
			if x["LOCK"]==lock_mp[lock_type]:
				X.append(x["THREADS"])
				Y.append(x["FAIRNESS"])
		plt.plot(X,Y,label = "%s " % (lock_mp[lock_type]))
		plt.legend()
	plt.show()
	
def fairness_test():
	filename = "fairness_result.txt"
	clean(filename)
	for lock_type in range(1,5):
		update_line("lock.h", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		update_line("lock.c", "#define LOCK_TYPE", "#define LOCK_TYPE %d" % (lock_type))
		os.system("make &&./timer fairness >> fairness_result.txt")		
	
if __name__ == '__main__':
	if sys.argv[1] == "list":
		list_test()
		plot_list()
	if sys.argv[1] == "hash":
		hash_test()
		plot_hash()
	if sys.argv[1] == "counter":
		counter_test()
		plot_counter()
	if sys.argv[1] == "twophase":
		twophase_test()
		plot_twophase()
	if sys.argv[1] == "fairness":
		fairness_test()
		plot_fairness()
