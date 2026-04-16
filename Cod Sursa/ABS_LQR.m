%Proiectare Regulator LQR 
t = double(DateSlipu0(:,1));
y = double(DateSlipu0(:,2));
u = double(t > 0); 
Ts = t(10) - t(9);

model_data = iddata(y, u, Ts);
M_n4sid = n4sid(model_data, 2);

Ad = M_n4sid.A;
Bd = M_n4sid.B;
Cd = M_n4sid.C;
Dd = M_n4sid.D;

Q = 10 * (Cd' * Cd);  % Penalizare pe ieșire
R = 0.2;              % Penalizare pe efort de control


K = dlqr(Ad, Bd, Q, R);

sys_cl = ss(Ad - Bd*K, Bd, Cd, 0, Ts);

Nx = inv(eye(size(Ad)) - Ad + Bd * K) * Bd;
Nbar = 1 / (Cd * Nx);


x = zeros(size(Ad,1),1);
y_sim = zeros(size(t));
r = 0.7 * ones(size(t));

for k = 1:length(t)
    u_k = -K * x + Nbar * r(k);

    u_k = min(max(u_k, 0), 1);

    x = Ad * x + Bd * u_k;
    y_sim(k) = Cd * x + Dd * u_k;
end


figure;
plot(t, u, 'LineWidth', 1); hold on;
plot(t, y, 'LineWidth', 1);
plot(t, y_sim,  'LineWidth', 1);

grid on;
legend('Semnal franare','Slip initial','Slip controlat - LQR');
title('Control Slip ABS LQR');
xlabel('Timp [s]');
ylabel('Slip');

