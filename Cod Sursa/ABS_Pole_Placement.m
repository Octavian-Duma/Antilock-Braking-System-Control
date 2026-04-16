% Proiectare regulator - Pole Placement  pe n4sid ,ordin 2
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

sys_ss = ss(Ad, Bd, Cd, Dd, Ts);
Hf = tf(sys_ss); 
%step(Hf);

poli_doriti = [0.85 0.9]; % poli stabili si rapizi
K = place(Ad, Bd, poli_doriti);


x = zeros(size(Ad,1),1);
y_sim = zeros(size(t));
r = 1* ones(size(t)); 

for k = 1:length(t)
    % u(k) regleaza spre r(k)
    e = r(k) - (Cd*x); % eroare de urmarire
    u_k = -K * x + e;  % actiune pe baza erorii

    % u_k = min(max(u_k, 0), 1);

    x = Ad*x + Bd*u_k;
    y_sim(k) = Cd*x + Dd*u_k;
end

figure;
plot(t, y); hold on;
plot(t, y_sim);
grid on;
legend('Slip masurat','Slip controlat - Pole Placement');
title('Control ABS folosind Pole Placement');
xlabel('Timp [s]');
ylabel('Slip');

%% % Proiectare regulator - Pole Placement pe n4sid ordin 2 optimizat cu pem

t = double(DateSlipu0(:,1));
y = double(DateSlipu0(:,2));
u = double(t > 0);
Ts = t(10) - t(9);
model_data = iddata(y, u, Ts);

M_n4sid_pem=pem(model_data,M_n4sid);

Ad_pem=M_n4sid_pem.A;
Bd_pem=M_n4sid_pem.B;
Cd_pem=M_n4sid_pem.C;
Dd_pem=M_n4sid_pem.D;

sys_ss_pem = ss(Ad_pem, Bd_pem, Cd_pem, Dd_pem, Ts);
Hf_pem= tf(sys_ss_pem);

poli_doriti = [0.85 0.97]; % poli stabili si rapizi
K = place(Ad_pem, Bd_pem, poli_doriti);


x = zeros(size(Ad_pem,1),1);
y_sim_pem = zeros(size(t));
r = 1 * ones(size(t)); 

for k = 1:length(t)
    e = r(k) - (Cd_pem*x); % eroare de urmarire
    u_k = -K * x + e;  % actiune pe baza erorii
    %u_k = min(max(u_k, 0), 1);
    x = Ad_pem*x + Bd_pem*u_k;
    y_sim_pem(k) = Cd_pem*x + Dd_pem*u_k;
end


figure;
plot(t, y); hold on;
plot(t, y_sim_pem);
grid on;
legend('Slip real','Slip controlat - Pole Placement');
title('Control Slip ABS folosind Pole Placement');
xlabel('Timp [s]');
ylabel('Slip');






