m = 1;
c = 1;
k = 1;

P = tf([1],[m,c,k]);

Kp = 10;
Ki = 5;
Kd = 8;

C = pid(Kp,Ki,Kd);

Gry = feedback(C*P,1);

step(Gry);

Ts = 0.05;
Pd= c2d(P,Ts);
