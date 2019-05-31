import sys
import numpy as np

print("python "+sys.argv[0]+" "+sys.argv[1]+" "+sys.argv[2])

f1=open(sys.argv[1],"r")
f2=open(sys.argv[2],"r")

l1=f1.readlines()
l2=f2.readlines()

print(str(len(l1))+" "+str(len(l2)))

cc=0.0;
for i in range(len(l1)):
    cc = cc + np.square(int(l1[i])-int(l2[i]))

cc = np.sqrt(cc)

print(cc)

