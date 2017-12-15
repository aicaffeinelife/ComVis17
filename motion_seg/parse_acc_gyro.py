import sys, os 
import numpy as np

# global variables 
time = [] 
acc_x = [] 
acc_y = [] 
acc_z = []
pitch = [] 
roll = []
yaw = []
# motion_periods = [0.1, 2.25, 6.25,7.50,10.1,12.5, 18.85, 24.25, 30.9, 33.05, 37.5, 38.8, 41.9, 44.05, 44.05, 48.8, 48.85, 52.55, 54, 59.45]

def load_acc_gyro_data(fname):
	with open(fname, 'r') as f:
		header = f.readline()
		lines = f.readlines()

	for line in lines:
		time.append(float(line.split('\t')[0]))
		acc_x.append(float(line.split('\t')[1]))
		acc_y.append(float(line.split('\t')[2]))
		acc_z.append(float(line.split('\t')[3]))
		pitch.append(float(line.split('\t')[4]))
		roll.append(float(line.split('\t')[5]))
		yaw.append(float(line.split('\t')[6]))
	return time, acc_x, acc_y, acc_z, pitch, roll, yaw


def np_convert():
	np_x = np.asarray(acc_x)
	np_y = np.asarray(acc_y)
	np_z = np.asarray(acc_z)
	np_p = np.asarray(pitch)
	np_r = np.asarray(roll)
	np_yaw = np.asarray(yaw)

	return np_x, np_y, np_z, np_p, np_r, np_yaw 

	




def check_data(data, thresh):
	chk_var = None
	if data[0] < thresh and data[1] < thresh and data[2] < thresh:
		chk_var = True 
	else:
		chk_var = False
	return chk_var



def segment_data(time, accel_thresh=1e-3, var_thresh=1e-2):
	window_sz = 2.5
	step_sz =  50
	var_accel = [] 
	var_gyro = []
	result = []
	
	np_x, np_y, np_z, np_p, np_r, np_yaw = np_convert()

	for idx in range(0, len(time), step_sz):
	
		win_start = idx 
		win_end = idx+step_sz
		var_x = np.var(np_x[win_start:win_end], axis=0)
		var_y = np.var(np_y[win_start:win_end], axis=0)
		var_z = np.var(np_z[win_start:win_end], axis=0)
		var_accel.append([var_x,var_y, var_z])

		var_p = np.var(np_p[win_start:win_end], axis=0)
		var_r = np.var(np_r[win_start:win_end], axis=0)
		var_yaw = np.var(np_yaw[win_start:win_end], axis=0)
		var_gyro.append([var_p, var_r, var_yaw])


	
	for acc,gyro in zip(var_accel, var_gyro):
		print(acc)
		print(gyro)
		print('----')
		acc_var  = check_data(acc, accel_thresh)
		gy_var = check_data(gyro, var_thresh)

		if acc_var and gy_var:
			result.append('rest')
		else: 
			result.append('motion')

	
		

	
	return result


def calc_gyro_vel(res, time):
	window_sz = 2.5
	del_t = 0.05
	step_sz = 50
	dist_gyro = []

	for i in range(len(res)):
		
		if(res[i] == 'motion'):
			i_start = i*step_sz 
			i_end = i_start + step_sz 
			# print("start:{}, end:{}".format(i_start, i_end))
			
			d_p = d_r = d_yaw = 0
			for j in range(i_start, i_end):
				
				d_p += pitch[j]*del_t 
				d_r += roll[j]*del_t 
				d_yaw += yaw[j]*del_t

				# d_p += v_p*del_t
				# d_r += v_r*del_t
				# d_yaw += v_yaw*del_t 

			dist_gyro.append([d_p, d_r, d_yaw])
	print(len(dist_gyro))

	return dist_gyro



def calc_accel_vel(res, time):
	vel_x = []
	vel_y = []
	vel_z = []
	

	dist_trav = [] 

	t_sample = 0.05 
	num_sample = 50
	win_sz = 2.5

	v_x_i = v_y_i = v_z_i = 0
	avg_x = avg_y = avg_z = 0

	for i in range(len(res)):
		if(res[i] == 'motion'):
			d_x = 0 
			d_y = 0
			d_z = 0
			start = i*num_sample
			end = start + num_sample
			for j in range(start, end):
				v_x_f = v_x_i + acc_x[j]*t_sample
				v_y_f = v_y_i + acc_y[j]*t_sample
				v_z_f = v_z_i + acc_z[j]*t_sample
				

			avg_x = (v_x_f + v_x_i)/2 
			avg_y = (v_y_f + v_y_i)/2 
			avg_z = (v_z_f + v_z_i)/2
			# print("Avg_x:{}, Avg_y:{}, Avg_z:{}".format(avg_x, avg_y, avg_z)) 
			v_x_i = v_x_f 
			v_y_i = v_y_f 
			v_z_i = v_z_f
			
			d_x += avg_x*win_sz 
			d_y += avg_y*win_sz 
			d_z += avg_z*win_sz
			dist_trav.append([d_x, d_y, d_z])
	
	return dist_trav



				
def write_results_nicely(dst_lst, fname):

	with open(fname, 'w') as wf:
		for i in range(len(dst_lst)):
			wf.write(str(dst_lst[i])+"\n")



def check_rest_motion(res):
	step_sz = 50
	for i in range(len(res)):
		id_start = i*step_sz 
		id_end  = id_start + step_sz 
		print("Res:{}, start:{}, end:{}".format(res[i], time[id_start], time[id_end]))




	


def main():
	fname = "acc_gyro.txt"
	load_acc_gyro_data(fname)
	res = segment_data(time)
	
	gy_dists = calc_gyro_vel(res, time)
	ac_dists = calc_accel_vel(res, time)

	# print(res)

	write_results_nicely(gy_dists, "gyro_results.txt")
	write_results_nicely(ac_dists, "acc_results.txt")
	# check_rest_motion(res)
	# print(len(ac_dists))
	


if __name__ == '__main__':
	main()
	
	

	
