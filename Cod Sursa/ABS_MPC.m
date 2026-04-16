%% %Achizitia datelor
t = DateSlipu0(:,1);
y = DateSlipu0(:,2);
u = (t > 0);
figure;

plot(t, u, 'LineWidth',1); hold on;
plot(t, y, 'LineWidth',1);
grid on;

xlabel('Timp [s]');               
ylabel('Valoare semnal');       
legend('u(t) – semnal frânare', 'y(t) – slip măsurat', 'Location','best');         
title(' Datele modelului ABS');


%% %Identificare model ABS

%Prelucrea datelor achizitionate
t = double(DateSlipu0(:,1));
y = double(DateSlipu0(:,2));
u = double(t > 0); 
Ts = t(10) - t(9);
model_data = iddata(y, u, Ts);
nz = 10;
u = [zeros(nz,1); u];
y = [zeros(nz,1); y];
t = [t(1) - Ts*(nz:-1:1)'; t];

%Creare model
sys_n4sid = n4sid(model_data, 2);
sys_n4sid_pem = pem(model_data,sys_n4sid);
sys_ss_pem = ss(sys_n4sid_pem.A, sys_n4sid_pem.B, ...
                sys_n4sid_pem.C, sys_n4sid_pem.D, Ts);

%Validare rezultate
figure;
resid(model_data,sys_ss_pem,10)
figure;
compare(model_data,sys_n4sid_pem); 

% Functie de transfer discreta
Hd = tf(sys_ss_pem);
disp('Functia de transfer discreta Hd(z):');
Hd;

% Functie de transfer continua (metoda Tustin)
Hc = d2c(Hd, 'tustin');
disp('Functia de transfer continua Hc(s):');
Hc;

% aspuns la treapta al modelului discret identificat
Hd = minreal(Hd);
figure;
step(Hd);
grid on;
title('Raspuns la treapta - model discret identificat');
xlabel('Timp [s]');
ylabel('Slip');

% Raspuns la treapta al modelului continuu identificat
Hc= minreal(Hc);
figure;
step(Hc);
grid on;
title('Raspuns la treapta - model continuu identificat');
xlabel('Timp [s]');
ylabel('Slip');



%% %Proiectare regulator MPC pentru ABS

t = double(DateSlipu0(:,1));
y = double(DateSlipu0(:,2));
u = double(t > 0);  
Ts = t(10) - t(9);  

nz = 10;
u = [zeros(nz,1); u];
y = [zeros(nz,1); y];
t = [t(1) - Ts*(nz:-1:1)'; t];

SlipRef = 0.7;
Eidf=0.77;
startIdx = find(u == 1, 1);

model_data = iddata(y, u, Ts);
sys_n4sid = n4sid(model_data, 2);
sys_n4sid_pem = pem(model_data, sys_n4sid);
sys_ss_pem = ss(sys_n4sid_pem.A, sys_n4sid_pem.B, ...
                sys_n4sid_pem.C, sys_n4sid_pem.D, Ts);

mpc_obj = mpc(sys_ss_pem, Ts);
mpc_obj.PredictionHorizon = 25;
mpc_obj.ControlHorizon = 2;
mpc_obj.Weights.ManipulatedVariables = 0;
mpc_obj.Weights.ManipulatedVariablesRate = 0.1;
mpc_obj.Weights.OutputVariables = 1;

mpc_obj.MV.Min = 0;
mpc_obj.MV.Max = 1;

N = length(t);
y_mpc = zeros(N, 1);    
u_mpc = zeros(N, 1);     
r     = zeros(N, 1);     

r(startIdx:end) = SlipRef;
x_mpc = mpcstate(mpc_obj);  

for k = 1:N
    yk = sys_ss_pem.C * x_mpc.Plant + sys_ss_pem.D * u(k);

    if k >= startIdx
        uk = mpcmove(mpc_obj, x_mpc, yk, r(k));
        uk = min(max(uk, 0), 1);
    else
        uk = 0;
    end

    x_mpc.Plant = sys_ss_pem.A * x_mpc.Plant + sys_ss_pem.B * uk;
    y_mpc(k) = sys_ss_pem.C * x_mpc.Plant + sys_ss_pem.D * uk;
    u_mpc(k) = uk;
end

figure;
plot(t, u, 'LineWidth',1),axis([-0.05 1.2 0 1.01]);hold on;
plot(t, y,'LineWidth',1); 
plot(t, y_mpc,'LineWidth',1);

legend('Semnal frânare','Slip initial', 'Slip controlat - MPC');
xlabel('Timp [s]');
ylabel('Slip');
title('Controlul slip-ului ABS cu MPC ');
grid on;

figure;
plot(t, u_mpc, 'LineWidth', 1);axis([-0.05 1.2 0 1.01]);hold on;
grid on;
xlabel('Timp [s]');
ylabel('Comanda MPC (u)');
title('Comanda regulatorului MPC');
legend('u_{MPC}');

%% %Modelul Controlerului
plant_mpc = mpc_obj.Model.Plant;
sys_plant = ss(plant_mpc);
%Spatiul starilor:
[A_mpc, B_mpc, C_mpc, D_mpc] = ssdata(sys_plant);


%Functia de transfer a controler-ului
Hd_mpc = tf(sys_plant);
Hc_mpc = d2c(Hd_mpc, 'tustin');
Hd_mpc;% in domeniu discret
Hc_mpc;% in domeniu continuu
%% Verificarea stabilitatii sistemului identificat
% bucla deschisa
figure;
margin(Hc); 
%bucla inchisa
H_0=feedback(Hc*Hc_mpc,1);
figure;
margin(H_0);
