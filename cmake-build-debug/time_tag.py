
f1c=open("TIME_TAG","r")
f2c=open("TIME_TAG_OUT","w")

lists = f1c.readlines()

line = lists[0].split(':')
sec_base = int(line[1])
usec_base = int(line[2])

#f2c.write('0 0\n')
aas = 0
aae = 0
rrs = 1
rre = 1

for list1 in lists:
	line = list1.split(':')
	sec = int(line[1])
	usec = int(line[2])
	sec = sec - sec_base
	usec = usec - usec_base
	if(usec<0):
		sec = sec - 1
		usec = usec + 1000000
	y = 0
	if(line[0]=='AssignCluster start'):
		y = aas
		aas = aas + 2
	if(line[0]=='AssignCluster finish'):
		y = aae
		aae = aae + 2
	if(line[0]=='Recenter start'):
		y = rrs
		rrs = rrs + 2
	if(line[0]=='Recenter finish'):
		y = rre
		rre = rre + 2
	f2c.write(str(sec * 1000000 + usec) + ' ' + str(y) + '\n')
