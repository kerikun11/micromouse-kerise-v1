clear;
%{
% curve90 (search)
v_const = 600;
pos_start = [0; 0; 0];
pos_end = [90; 90; pi/2];
%}
%%{
% curve90 (fast)
v_const = 920;
pos_start = [0; 0; 0];
pos_end = [90; 90; pi/2];
%}
%{
% curve45 (fast)
v_const = 2000;
pos_start = [0; 0; 0];
pos_end = [180; 90; pi/4];
%}
%{
% curveV90 (fast)
v_const = 1300;
pos_start = [0; 0; 0];
pos_end = [90*sqrt(2); 90*sqrt(2); pi/2];
%}
%{
% curve135 (fast)
v_const = 872*180/(52.5251+126.8067);
pos_start = [0; 0; 0];
pos_end = [0; 180; pi*3/4];
%}
%{
% curve180 (fast)
v_const = 1300*180/168.4119;
pos_start = [0; 0; 0];
pos_end = [0; 180; pi];
%}

omega_max = 6 * pi;
dt = 1/v_const/100;

T = (pos_end(3)-pos_start(3))/(1/2 - 1/(4*pi)*cos(2*pi-pi/2))/omega_max;
t = 0:dt:T;
omega = omega_max * (0.5 + 0.5*sin(2*pi/T*t-pi/2));
theta = omega_max * (1/2*t - T/(4*pi)*cos(2*pi/T*t-pi/2));

pos_cur = pos_start;
pos = pos_cur';

for i = 1:size(theta, 2)
    R = [cos(theta(i)), -sin(theta(i)), 0;
        sin(theta(i)), cos(theta(i)), 0
        0, 0, 1];
    pos_cur = pos_cur + R*[v_const; 0; 0]*dt;
    pos_cur(3) = pos_start(3) + theta(i);
    pos = [pos; pos_cur'];
end

straight_mat = [cos(pos_start(3)), cos(pos_end(3)), 0;
    sin(pos_start(3)), sin(pos_end(3)), 0;
    0, 0, 1];

if abs(det(straight_mat)) < 1e-12
    straight_len = [0;0];
else
    straight_len = inv(straight_mat)*[pos_end-pos_start-pos(end,:)'];
end

straight_1st = 0:v_const*dt:straight_len(1);
straight_3rd = 0:v_const*dt:straight_len(2);

pos_1st = [pos_start(1)+straight_1st*cos(pos_start(3));
    pos_start(2)+straight_1st*sin(pos_start(3));
    pos_start(3)+straight_1st*0]';

pos_2nd = ones(size(pos, 1), 1)*pos_1st(end, :)+pos;

pos_3rd = [pos_2nd(end, 1)+straight_3rd*cos(pos_end(3));
    pos_2nd(end, 2)+straight_3rd*sin(pos_end(3));
    pos_2nd(end, 3)+straight_3rd*0]';

pos_1st = pos_1st(1:size(pos_1st,1)-1, :);
pos_3rd = pos_3rd(2:size(pos_3rd,1), :);

subplot(4,1,1);
plot(t, omega, '.', 'MarkerSize', 6); grid on;
title('Šp‘¬“x', 'FontSize', 11);
xlabel('t', 'FontSize', 11);
ylabel('\omega', 'FontSize', 11);
subplot(4,1,2);
plot(t, theta, '.', 'MarkerSize', 6); grid on;
title('Žp¨Šp', 'FontSize', 11);
xlabel('t', 'FontSize', 11);
ylabel('\theta', 'FontSize', 11);

subplot(4,1,[3,4]);
hold off;
plot(pos_1st(:,1),pos_1st(:,2), '.', 'MarkerSize', 6); hold on;
plot(pos_2nd(:,1),pos_2nd(:,2), '.', 'MarkerSize', 6);
plot(pos_3rd(:,1),pos_3rd(:,2), '.', 'MarkerSize', 6);
title('ˆÊ’u', 'FontSize', 11);
grid on;
xlabel('x', 'FontSize', 11);
ylabel('y', 'FontSize', 11);

pos_all = [pos_1st; pos_2nd; pos_3rd];
pos_all = pos_all(1:100:size(pos_all, 1),:);
format short;
csvwrite('data.csv',pos_all);

pos_all(end, :)
size(pos_all, 1)
