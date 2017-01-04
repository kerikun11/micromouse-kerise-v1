clear;
gain = 1;
v_const = 1000 * gain;
omega_start = 0;
omega_end = 0;
omega_dot = gain * gain * pi * 96;
omega_max = gain * pi * 6;
pos_start = [0, 0, 0];
pos_end = [0, 180, pi/2];
dt = 0.001;

t1 = (omega_max - omega_start)/omega_dot;
t3 = (omega_max - omega_end)/omega_dot;
t2 = (pos_end(3)-(omega_start+omega_max)/2*t1-(omega_max+omega_end)/2*t3)/omega_max;

