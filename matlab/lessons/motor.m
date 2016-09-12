J = (0.58)*(1e-3)*(1e-4);
R = 4.3;
L = 65e-6;
Km = 3.96*(1e-3);
Ke = 0.414*(1e-3)/60*2*pi;

P = tf([1], [L*J/Km, R*J/Km, Ke]);

step(P);

Ts = 1e-3;
Pd = c2d(P, Ts);

pidTuner(Pd);
