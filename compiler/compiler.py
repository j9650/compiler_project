import sys
import numpy as np

print("python "+sys.argv[0]+" "+sys.argv[1]+" "+sys.argv[2]+" "+sys.argv[3]+" "+sys.argv[4])
f1c=open(sys.argv[1],"r")
f2c=open(sys.argv[3],"w")
f1h=open(sys.argv[2],"r")
f2h=open(sys.argv[4],"w")

programc=f1c.readlines()
programh=f1h.readlines()
program=programc+programh
clines = len(programc)

i=0;
for line in program:
	if (line[-1] == '\n'):
		line = line[:-1]
	st = line.split('//')[0]
	program[i] = st
	i = i + 1

#tmp = program
#program = []
#for line in tmp:
#	if(line.find(';') != -1):
#		st = line.split(';')
#		for ss in range(len(st)-1):
#			program.append(st[ss]+';')
#		if(len(st[-1])>0):
#			program[-1] = program[-1]+st[-1]
#	else:
#		program.append(line)

#i=0;
#tt = 0;
#for line in program:
#	for k in range(len(line)-1):
#		if()
#	if (line[-1] == '\n'):
#		line = line[:-1]
#	st = line.split('//')[0]
#	program[i] = st
#	i = i + 1

class_list = []
class_pos = []
for k in range(len(program)):
	line = program[k]
	if(line.find('class ') != -1):
		i = line.find('class')+5
		while(line[i] == ' '):
			i = i+1
		class_name = ''
		while(i<len(line) and line[i]!=' ' and line[i]!=':' and line[i]!='{'):
			class_name = class_name + line[i]
			i = i+1
		if(k>0):
			if(program[k-1].find('__Fluid__') != -1):
				class_list.append(class_name)
				class_pos.append(line)

i = 0;

f_tmpc = open('tmp.cpp',"w")
f_tmph = open('tmp.h',"w")
##deal with the #pragma
while(i<len(program)):
	this_line_res = ''
	line = program[i]
	shrink = line[:line.find(line.strip())]
	if(line.find('#pragma') != -1):
		#print(line)
		if(line.find('#pragma CallCount') != -1):
			pos = line.find('#pragma CallCount') + 17
			st = line[line.find('{',pos)+1:line.rfind('}',pos)]
			#i = line.find()
			#for i in range(pos, len())
			if(st.find('__call_count__') != -1): #__call_count__ 14
				if(st.find('=') != -1):
					this_line_res = shrink+st.replace('__call_count__', 'auto')
				else:
					stt = st[st.find('__call_count__')+14:].split()
					ii=0
					#print(stt)
					while(len(stt[ii]) == 0):
						ii = ii+1
					var_name = stt[ii]
					if(var_name[-1] == ';'):
						var_name = var_name[:-1]
					this_line_res = shrink+'int *'+var_name+'_counter = new int(0);\n' + shrink + 'auto '+var_name+' = new __fluid__<int>('+var_name+'_counter);'
			else:
				if((st.find('(') != -1) and (st.find(')') != -1)):
					this_line_res = shrink + '{\n' + shrink + '\t' + st +'\n' + shrink + '\t(*counter)++;\n' + shrink + '}'
				else:
					this_line_res = shrink+st
			#print(this_line_res+'\n')

		if(line.find('#pragma valve') != -1):
			pos = line.find('#pragma valve') + 13
			st = line[line.find('{',pos)+1:line.find('}',pos)]
			#print(st+'\n')
			stt = st.split()
			ii=0
			while(len(stt[ii]) == 0):
				ii = ii+1
			valve_name = stt[ii]
			ii=ii+1
			while(len(stt[ii]) == 0):
				ii = ii+1
			var_name = stt[ii]
			if(var_name[-1] == ';'):
				var_name = var_name[:-1]

			this_line_res = shrink+valve_name+'<int> '+var_name+';'
			#print(this_line_res+'\n')

		if(line.find('#pragma Stable') != -1):
			pos = line.find('#pragma Stable')
			st = line[pos:line.rfind('}',pos)+1]
			pos = line.find('#pragma Stable') + 14
			stt = line[line.find('{',pos)+1:line.find('}',pos)]
			this_line_res = line.replace(st,stt+'\n'+shrink+'int stable;')
			#print(st)
			#print(stt)
			#print(this_line_res+'\n')
			#print(st+'\n')

		program[i] = this_line_res	
	if(i<clines):
		f_tmpc.write(program[i]+'\n')
	else:
		f_tmph.write(program[i]+'\n')

	i = i + 1

f_tmph.close()
f_tmpc.close()

##scope analysis
f_tmpc = open('tmp.cpp',"r")
f_tmph = open('tmp.h',"r")
programc=f_tmpc.readlines()
programh=f_tmph.readlines()
program=programc+programh
clines = len(programc)
f_tmph.close()
f_tmpc.close()
scope_lv = 0
scope_name = []

i = 0
for line in program:

	st = line.rstrip()
	program[i] = st
	scope_name.append('')
	i = i + 1

i = 0
lv1 = 1
lv2 = 1
curr_scope = ''
func_list = []
f_tmp = open('tmp.cpp',"w")
while(i < len(program)):
	line = program[i]
	pre_scope_lv = scope_lv
	for ii in range(len(line)):
		if(line[ii]=='\'' and (ii==1 or line[ii-1] != '\\')):
			lv1 = lv1 * -1
		if(line[ii]=='\"' and (ii==1 or line[ii-1] != '\\')):
			lv2 = lv2 * -1
		if(line[ii]=='{' and lv1>0 and lv2>0):
			scope_lv = scope_lv+1
		if(line[ii]=='}' and lv1>0 and lv2>0):
			scope_lv = scope_lv-1
	if(scope_lv == 0):
		curr_scope = ''

	if(line.find('__Fluid__') != -1):
		st = program[i+1]
		if(st.find('class') != -1):
			pos = st.find('class')+5
			stt = st[pos:].split()
			st = stt[0]
			pos = 0
			class_name = ''
			while(pos<len(st) and st[pos]!=' ' and st[pos]!=':' and st[pos]!='{' and st[pos]!=';' and st[pos]!='\n'):
				class_name = class_name + st[pos]
				pos = pos+1
			curr_scope = class_name
			#scope_name[i] = curr_scope

	if(line.find('::')!=-1 and pre_scope_lv == 0):
		st = line.strip()
		stt = st.split('::')
		for k in range(len(stt)-1):
			ss = stt[k].strip()
			var_name = ss.split()[-1]
			if(var_name in class_list):
				curr_scope = var_name
				func_list.append(line)

	scope_name[i] = curr_scope
	#if(program[i] == '\n'):
	f_tmp.write(program[i]+' // '+scope_name[i] + '\n')
	#f_tmp.write(program[i]+'\n')
	i = i+1

f_tmp.close()

##Generate the task code
i = 0
global_valve = 0
standard_valves = ['ValveGT', 'ValveEQ', 'ValveLT']
insert_func = []
insert_class = []
while(i < len(program)):
	line = program[i]
	if(line.find('<<<') != -1 and line.find('>>>') != -1):
		print(line)
		this_line_res = ''
		shrink = line[:line.find(line.strip())]
		kernel = line[line.find('<<<')+3:line.find('>>>')]
		func = line[line.find('>>>')+3:].strip()

		guard = kernel.split(',',1)[0]
		guard = guard.strip()

		st = kernel.split(',',1)[1]
		st = st.strip()
		j=0;
		while(st[j] != '{'):
			j = j + 1
		lv=1
		valve_st = st[j+1:]
		pos = j + 1
		j=0
		while(j<len(valve_st) and lv>0):
			if(valve_st[j] == '{'):
				lv = lv + 1
			if(valve_st[j] == '}'):
				lv = lv - 1
			j = j + 1
		if(lv == 0):
			valve_st = valve_st[0:j-1].strip()
			kernel = st[j+pos:] ##update kernel
		#print(valve_st)
		#print(kernel)
		valve_list = [-1]
		j = 0
		lv = 0
		while(j<len(valve_st)): ##possible have " or '
			if(valve_st[j]=='('):
				lv = lv + 1
			if(valve_st[j]==')'):
				lv = lv - 1
			if(valve_st[j]==',' and lv == 0):
				valve_list.append(j)
			j = j + 1
		valve_list.append(j)

		valves = '{'
		stable_flag = 0
		valve_name = ''
		valve_para = ''
		for j in range(len(valve_list)-1):
			stt = valve_st[valve_list[j]+1:valve_list[j+1]].strip()
			valve_name = stt.split('(')[0].strip()
			valve_para = stt[stt.find(valve_name)+len(valve_name):].strip()

			if(valve_name in standard_valves):
				this_line_res = this_line_res+shrink+'auto v'+str(global_valve)+'_ = new '+valve_name+'<int>'+valve_para+';\n'
				valves = valves + 'v'+str(global_valve)+'_, '
				global_valve = global_valve + 1
			elif(valve_name == 'ValveST'):
				stable_flag = 1
			elif(valve_name != ''):
				this_line_res = this_line_res+shrink+'auto v'+str(global_valve)+'_ = '+valve_name+'.init'+valve_para+';\n'
				valves = valves + 'v'+str(global_valve)+'_, '
				global_valve = global_valve + 1

		if(stable_flag == 1):
			valve_para = valve_para[1:-1].strip()
			ii=0
			lv=0
			tt=0
			while(ii<len(valve_para) and tt==0):
				if(valve_para[ii]=='('):
					lv = lv + 1
				if(valve_para[ii]==')'):
					lv = lv - 1
				if(valve_para[ii]==',' and lv == 0):
					tt = 1
				ii = ii + 1
			ii = ii - 1
			stable_add = valve_para[:ii]
			theshold = valve_para[ii+1:].strip()
			if(stable_add.rfind('->') > stable_add.rfind('.')):
				stable_sta = stable_add[:stable_add.rfind('->')+2]+'stable'
			else:
				stable_sta = stable_add[:stable_add.rfind('.')+1]+'stable'

			this_line_res = this_line_res + shrink + 'if(('+stable_sta+' <= '+theshold+')||(RAND_ST)) {\n'
			this_line_res = this_line_res + shrink + '\tauto tmp = '+stable_add+';\n'
			this_line_res = this_line_res + shrink + '\t'+func+'\n'
			this_line_res = this_line_res + shrink + '\tif('+stable_add+' == tmp)\n'
			this_line_res = this_line_res + shrink + '\t\t'+stable_sta+'++;\n'
			this_line_res = this_line_res + shrink + '\telse\n'
			this_line_res = this_line_res + shrink + '\t\t'+stable_sta+' = 0;\n'+shrink+'}\n'+shrink+'else\n'+shrink+'\t'+stable_sta+'++;\n'
		else:
			if(valves == '{'):
				valves = valves + '}'
			else:
				valves = valves[:-2] + '}' ###done with valves

			kernel = kernel[kernel.find(',')+1:].strip()
			producer = kernel[kernel.find('{'):kernel.find('}')+1]
			kernel = kernel[kernel.find('}')+1:].strip()
			call_num = ''
			if(kernel.find(',') != -1):
				call_num = kernel[kernel.find(',')+1:].strip()
			else:
				call_num = ''
			func_name = func[:func.find('(')].strip()
			func_para = func[func.find('(')+1:func.rfind(')')].strip()

			if(call_num != ''):
				insert_func.append(func_name+';;;;;;'+scope_name[i])
				#insert_class.append(scope_name[i])
				this_line_res = this_line_res + shrink + 'auto tpb__'+guard+' = std::bind(&'+scope_name[i]+'::'+func_name+', this, '+func_para+', std::placeholders::_1);\n'
				this_line_res = this_line_res + shrink + 'auto tp__'+guard+' = tf->newTask<decltype(tpb__'+guard+'), int*>("Task", tpb__'+guard+', '+call_num+'->p);\n'
				this_line_res = this_line_res + shrink + 'Guard*       '+guard+' = gs->newGuard("Guard", '+valves+', tp__'+guard+', '+producer+');\n'
			else:
				this_line_res = this_line_res + shrink + 'auto tpb__'+guard+' = std::bind(&'+scope_name[i]+'::'+func_name+', this, '+func_para+');\n'
				this_line_res = this_line_res + shrink + 'auto tp__'+guard+' = tf->newTask<decltype(tpb__'+guard+')>("Task", tpb__'+guard+');\n'
				this_line_res = this_line_res + shrink + 'Guard*       '+guard+' = gs->newGuard("Guard", '+valves+', tp__'+guard+', '+producer+');\n'

		program[i] = this_line_res





		#print(valves)

		print(this_line_res)


	i = i + 1


##insert call_num in function
func_set = set(insert_func)
func_list2 = list(func_set)
print('============================')

for i in range(len(func_list2)):
	func_name = func_list2[i].split(';;;;;;')[0]
	class_name = func_list2[i].split(';;;;;;')[1]
	print(func_name+';;;'+class_name)

	#change in class claim
	pos_st = class_pos[class_list.index(class_name)].rstrip()
	#print pos_st
	#print(pos_st.split('a'))
	ii = 0
	tt = 0
	while(ii<len(program) and tt == 0):
		if(program[ii].find(pos_st)!=-1):
			tt = 1
			#print(program[ii].split('a'))
		ii = ii + 1
	ii = ii - 1

	tt=0
	lv = 0
	lv1 = 1
	lv2 = 1
	while(ii<len(program) and tt == 0):
		line = program[ii]
		if(line.find(func_name)!=-1 and lv <= 1):
			tt = 1
		for iii in range(len(line)):
			if(line[iii]=='\'' and (iii==1 or line[iii-1] != '\\')):
				lv1 = lv1 * -1
			if(line[iii]=='\"' and (iii==1 or line[iii-1] != '\\')):
				lv2 = lv2 * -1
			if(line[iii]=='{' and lv1>0 and lv2>0):
				lv = lv+1
			if(line[iii]=='}' and lv1>0 and lv2>0):
				lv = lv-1
		ii = ii + 1
	
	ii = ii - 1
	st = program[ii]
	#print(st)
	st = st[:st.rfind(')')] + ', int *counter);'	
	print(st)
	program[ii] = st

	ii = 0
	tt = 0
	while(ii<len(program) and tt == 0):
		line = program[ii]
		stst = line.split()
		st = ''
		for line in stst:
			st = st + line
		if(st.find(class_name+'::'+func_name) != -1):
			tt = 1
		ii = ii + 1
	ii = ii - 1
	st = program[ii]
	st = st[:st.rfind(')')] + ', int *counter' + st[st.rfind(')'):]
	print(st)
	program[ii] = st
		
##api and delete __Fluid__
for i in range(len(program)):
	line = program[i]

	if(line.find('__Fluid__')!=-1):
		program[i] = ''
	elif(line.find('Task::init')!=-1):
		agg = line[line.find('(')+1:line.rfind(')')]
		shrink = line[:line.find(line.strip())]
		this_line_res = shrink + 'auto tf = new TaskFactory();\n'
		this_line_res = this_line_res + shrink + 'auto gs = new AggressiveGS('+agg+');\n'
		program[i] = this_line_res
	elif(line.find('Task::sync();')!=-1):
		shrink = line[:line.find(line.strip())]
		this_line_res = shrink + 'auto tpend = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);\n' + shrink + 'gs->sync(tpend);\n'
		program[i] = this_line_res
	
	

	
	


for i in range(len(class_list)):
	print(class_list[i] + ' ' + str(class_pos[i]))

#for line in program:
#	f2.write(line+'\n')

for i in range(len(program)):
	line = program[i]
	if(i<clines):
		f2c.write(line+'\n')
	else:
		f2h.write(line+'\n')