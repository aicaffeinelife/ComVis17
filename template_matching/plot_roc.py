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


fig = plt.figure()
ax = fig.add_subplot(111)
plt.plot(tpr, fpr, color='navy')

for i in range(len(tpr)):
	ax.annotate(thresh[i], (tpr[i], fpr[i]))


plt.xlim([0.0, 1.0])
plt.ylim([0.0, 1.0])
plt.xlabel('False Positive Rate')
plt.ylabel('True Positive Rate')
plt.title('ROC curve')
fig.savefig('ROC_curve.png')


	

