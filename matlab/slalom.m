clear;
%{
% curve135
gain = 1;
v_const = 1000 * gain;
omega_dot = (48 * gain * gain) * 5;
omega_max = (2 * gain) * pi * 3;
pos_start = [0; 0];
pos_end = [0; 180];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4*3;
dt = 0.001;
%}
%{
% curve135R
gain = 1;
v_const = 1000 * gain;
omega_dot = (48 * gain * gain) * 5;
omega_max = (2 * gain) * pi * 3;
pos_start = [0; 0];
pos_end = [180*sqrt(2)/2; 180*sqrt(2)/2];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4*3;
dt = 0.001;
%}
%{
% curve90
gain = 1;
v_const = 1000 * gain;
omega_dot = (48 * gain * gain) * pi * 4;
omega_max = (2 * gain) * pi * 2.4;
pos_start = [0; 0];
pos_end = [90; 90];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/2;
dt = 0.001;
%}
%{
% curve60R
gain = 1;
v_const = 1000 * gain;
omega_dot = (300 * gain * gain) * pi;
omega_max = (24 * gain) * pi;
pos_start = [0; 0];
pos_end = [90*cos(-pi/6)-90*sin(-pi/6); 90*sin(-pi/6)+90*cos(-pi/6)];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/3;
dt = 0.001;
%}
%{
% curve45
gain = 1;
v_const = 1000 * gain;
omega_dot = (48 * gain * gain) * 2;
omega_max = (2 * gain) * pi * 1;
pos_start = [0; 0];
pos_end = [270; 90];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4;
dt = 0.001;
%}
%{
% curve45R
gain = 1;
v_const = 1000 * gain;
omega_dot = (48 * gain * gain) * 2;
omega_max = (2 * gain) * pi * 1;
pos_start = [0; 0];
pos_end = [90*sqrt(2)/2*4; 90*sqrt(2)/2*2];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4;
dt = 0.001;
%}
%%{
% curve180
v_const = 1000;
omega_dot = 32 * pi;
omega_max = 4 * pi;
pos_start = [0; 0];
pos_end = [0; 180];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi;
dt = 0.001;
%}

[theta, omega] = trapezoid(omega_dot, omega_max, omega_start, omega_end, theta_end-theta_start, dt);
theta = theta + theta_start;

N = length(theta);
t = 0:dt:dt*(N-1);

figure(1);
subplot(2,1,1); 
plot(t,theta,'.');
grid on;

subplot(2,1,2); 
plot(t,omega,'.');
grid on;

pos_cur = pos_start;
pos = pos_cur';

for i = 1:N
    R = [cos(theta(i)), -sin(theta(i)); sin(theta(i)), cos(theta(i))];
    pos_cur = pos_cur + R*[v_const;0]*dt;
    pos = [pos; pos_cur'];
end

x_end = pos(end,1);
y_end = pos(end,2);
straight_mat = [cos(theta_start), cos(theta_end); sin(theta_start), sin(theta_end)];
if abs(det(straight_mat)) < 1e-12
    straight_len = [0;0];
else
    straight_len = inv(straight_mat)*[pos_end-pos(end,:)'];
end

straight_len_1st = straight_len(1);
straight_len_2nd = straight_len(2);

N_straight_1st = floor(straight_len_1st / v_const / dt);
N_straight_2nd = floor(straight_len_2nd / v_const / dt);
pos_straight_1st = 0:v_const*dt:v_const*dt*(N_straight_1st-1);
pos_straight_2nd = 0:v_const*dt:v_const*dt*(N_straight_2nd-1);
figure(2);
grid on;
hold on;
plot(pos_straight_1st'*cos(theta_start), pos_straight_1st'*sin(theta_start), '.', 'MarkerSize', 10);
plot(straight_len_1st*cos(theta_start)+pos(:,1), straight_len_1st*sin(theta_start)+pos(:,2), '.', 'MarkerSize', 10);
plot(straight_len_1st*cos(theta_start)+x_end+pos_straight_2nd'*cos(theta_end), straight_len_1st*sin(theta_start)+y_end+pos_straight_2nd'*sin(theta_end),'.','MarkerSize',10);

tr = [pos_straight_1st', 0*ones(N_straight_1st,1), 0*ones(N_straight_1st,1);
    pos(:,1)+straight_len_1st ,pos(:,2), [0; theta];
    straight_len_1st+pos_straight_2nd'*cos(theta_end)+x_end, pos_straight_2nd'*sin(theta_end)+y_end, theta_end*ones(N_straight_2nd,1)];

size(tr)