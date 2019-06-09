# coding: utf-8

import sys

def hexex(num, n):
	return "0x" + hex(num)[2:].zfill(n)

def inp(msg, t, t_msg, f, f_msg):
	var = raw_input(msg + "(" + t + "\\" + f + "): ")
	if var.lower() == t.lower():
		print(t_msg)
	elif var.lower() == f.lower():
		print(f_msg)
	else:
		print("Error " + msg + ", set default value = " + t)
		var = t
	return var


# Address
rmas_s = raw_input("Real mode address (0x0000:0x0000 - 0xffff:0x000f): ").split(":")
try:
	rma_h = int(rmas_s[0], 16)
	rma_l = int(rmas_s[1], 16)
except ValueError:
	print("Error real mode address. Setting default = 0x4b10:0x0000")
	rma_h = 0x4b10
	rma_l = 0x0000
lba = rma_h * 16 + rma_l
print("Linear address: " + hexex(lba, 8))

# Segment limit
seg_lim = 0
try:
	seg_lim = int(raw_input("Segment limit (0x0 - 0xffff): "), 16)
except ValueError:
	print("Error segment limit. Setting default = 0x0fff")
	seg_lim = 0x0fff
print("Limit: " + hexex(seg_lim, 4))

rw = "y"
ce = "32b"
db = "n"

# type
type = raw_input("Code or data segment (c\d): ")
if type.lower() == 'c':
	print("Type: code")
	rw = inp("Readable", "y", "Readable", "n", "Non-Readable")
	db = inp("Default", "32b", "Default = 32b", "16b", "Default = 16b")
	ce = inp("Conforming", "n", "Non-Conforming", "y", "Conforming")
elif type.lower() == 'd':
	print("Type: data")
	rw = inp("Writable", "y", "Writable", "n", "Non-Writable")
	db = inp("Expantion direction", "up", "Expantion direction up", "down", "Expantion direction down")
	ce = inp("Big", "n", "max offset = 0000ffffh", "y", "max offset = 0ffffffffh")
else:
	print("Error type, default: code")
	type = 'c'
	rw = inp("Readable", "y", "Readable", "n", "Non-Readable")
	db = inp("Default", "32b", "Default = 32b", "16b", "Default = 16b")
	ce = inp("Conforming", "n", "Non-Conforming", "y", "Conforming")

# DPL
dpl = 0
try:
	dpl = int(raw_input("Descriptor privilege level (0-3): "))
except ValueError:
	print("Error DPL, setting default = 0")
	dpl = 0
if dpl == 0:
	print("DPL: Level 0 (Kernel)")
elif dpl == 1:
	print("DPL: Level 1")
elif dpl == 2:
	print("DPL: Level 2")
elif dpl == 3:
	print("DPL: Level 3 (User mode)")
else:
	print("Error DPL, setting default = 0")
	dpl = 0
	
# Granularity
gran = inp("Granularity", "1B", "Granularity: 1B", "4kB", "Granularity: 4kB")

# Present
pres = inp("Present", "y", "Present", "n", "Non-present")

# Accessed
acc = inp("Accessed", "n", "Non-Accessed", "y", "Accessed")

# AVL
avl = inp("AVL", "n", "Non-avaliable for use the system software", "y", "Avaliable for use the system software")

descriptor = "dw "
descriptor += hexex(seg_lim, 4)
descriptor += ", 0x"
descriptor += hexex(lba, 8)[6:]
descriptor += ", 0x"
tempb = ""

if pres.lower() == "y":
	tempb = "1"
else:
	tempb = "0"

tempb += bin(dpl)[2:].zfill(2)
tempb += "1"

if type.lower() == 'c':
	tempb += "1"
else:
	tempb += "0"

if ce.lower() == "down" or ce.lower() == "y":
	tempb += "1"
else:
	tempb += "0"

if rw.lower() == 'y':
	tempb += "1"
else:
	tempb += "0"

if acc.lower() == "y":
	tempb += "1"
else:
	tempb += "0"

descriptor += hex(int(tempb, 2))[2:].zfill(2)
descriptor += hexex(lba, 8)[4:6]
descriptor += ", 0x"
descriptor += hexex(lba, 8)[2:4]

if gran.lower() == "4kB":
	tempb = "1"
else:
	tempb = "0"

if db == "16b" or db == "n":
	tempb += "0"
else:
	tempb += "1"

tempb += "0"

if avl.lower() == "n":
	tempb += "0"
else:
	tempb += "1"

tempb += "0000"

descriptor += hex(int(tempb, 2))[2:].zfill(2)

print
print(";---------------------------------")
print(descriptor)
print(";---------------------------------")