clear;
omega_dot = pi * 96;
omega_max = pi * 6;
%%{
% curve90 (search)
v_const = 800;
pos_start = [0; 0];
pos_end = [90; 90];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/2;
%}
%{
% curve45
v_const = 1680;
pos_start = [0; 0];
pos_end = [180; 90];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4;
%}
%{
% curve45R
v_const = 1680;
pos_start = [0; 0];
pos_end = [90*sqrt(2)/2*3; 90*sqrt(2)/2*1];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4;
%}
%{
% curve45_45
v_const = 1460;
pos_start = [0; 0];
pos_end = [180*1.41421356/2; 180*1.41421356/2];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/2;
%}
%{
% curve45_S_45
v_const = 2900;
pos_start = [0; 0];
pos_end = [360*1.41421356/2; 360*1.41421356/2];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/2;
%}
%{
% curve90 (fast)
v_const = 1030;
pos_start = [0; 0];
pos_end = [90; 90];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/2;
%}
%{
% curve135
v_const = 1080;
pos_start = [0; 0];
pos_end = [0; 180];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4*3;
%}
%{
% curve135R
v_const = 1080;
pos_start = [0; 0];
pos_end = [180*sqrt(2)/2; 180*sqrt(2)/2];
omega_start = 0;
omega_end = 0;
theta_start = 0;
theta_end = pi/4*3;
%}
%{
% curve180
v_const = 1611;
pos_start = [0; 0];
pos_end = [180; 90];
omega_start = 0;
omega_end = omega_max;
theta_start = 0;
theta_end = pi/2;
%}

dt = 0.01/v_const;

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

%{
ftr = flipud(tr);
ftr(:,2) = 180-ftr(:,2);
ftr(:,3) = pi-ftr(:,3);

tr = [tr;ftr];

figure();
plot(tr(:,1),tr(:,2), '.');
figure();
plot(tr(:,3), '.');
%}

tr = tr(1:100:size(tr, 1),:);
format short;
csvwrite('data.csv',tr);

size(tr)
tr(end,:)
pos_end
