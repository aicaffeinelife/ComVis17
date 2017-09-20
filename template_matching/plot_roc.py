# script for plotting ROC curve. 
# Written by: Ankit Kulshrestha

import os, sys 
import matplotlib.pyplot as plt 


source_f = os.path.join(os.getcwd(), "detections.txt")
thresh = [] 
tpr = [] 
fpr = [] 
lines = open(source_f,'r').read().splitlines()

for line in lines:
	thresh.append(int(line.split(' ')[0]))
	tpr.append(float(line.split(' ')[1]))
	fpr.append(float(line.split(' ')[2]))



print(len(fpr))
print(len(tpr))
fig = plt.figure()
ax = fig.add_subplot(111)


plt.plot(fpr, tpr, color='orange')

for i in range(len(thresh)):
	if fpr[i] >= 1.0:
		ax.annotate(' ', (fpr[i], tpr[i]))
	else:
		ax.annotate(thresh[i], (fpr[i], tpr[i]))


plt.xlim([0.0, 1.0])
plt.ylim([0.0, 1.1])
plt.xlabel('False Positive Rate')
plt.ylabel('True Positive Rate')
plt.title('ROC curve')
fig.savefig('ROC_curve_4.png')


	

