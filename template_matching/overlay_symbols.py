import os 
import matplotlib.pyplot as plt
import matplotlib.image as mpimg


img = mpimg.imread('image_crop_25_89_thin.png')
fig = plt.figure()
ax = fig.add_subplot(111)
ax.annotate('x',(4,2))
plt.xlim([0,9])
plt.ylim([0,15])
plt.axis('off')
plt.savefig('image_crop_25_89_thin_overlay.png')
