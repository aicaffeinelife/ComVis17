# script for plotting ROC curve. 
# Written by: Ankit Kulshrestha

import os, sys 
import matplotlib.pyplot as plt 


source_f = os.path.join(os.getcwd(), "detections_improved.txt")
plt_name = sys.argv[1]
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
	if fpr[i] >= 0.135:
		ax.annotate(' ', (fpr[i], tpr[i]))
	else:
		ax.annotate(thresh[i], (fpr[i], tpr[i]))


plt.xlim([0.0, 0.15])
plt.ylim([0.0, 0.9])
plt.xlabel('False Positive Rate')
plt.ylabel('True Positive Rate')
plt.title('ROC curve')
fig.savefig(plt_name)


	

