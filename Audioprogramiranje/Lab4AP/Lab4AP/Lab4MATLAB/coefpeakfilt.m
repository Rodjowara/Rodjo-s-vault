function [b0 b1 b2 a1 a2] = coefpeakfilt (fc, fb, G,fs) 
%proracun koeficijenata po jednadžbama za pojačanje i prigušenje
% Wc normalizirana centralna frekvencija 0<Wc<1, i.e. 2*fc/fS. 
% Wb normaliziran aširina pojasa  0<Wb<1, i.e. 2*fb/fS. 
% G Pojačanje u dB.
K=tan(pi*fc/fs);
V0 = 10^(G/20); 
Q=fc/fb; %postotak 
if G >= 0
b0=(1+(V0/Q)*K+K^2)/(1+(1/Q)*K+K^2);
b1=2*(K^2-1)/(1+1/Q*K+K^2);
b2=(1-V0/Q*K+K^2)/(1+1/Q*K+K^2);
a1=2*(K^2-1)/(1+1/Q*K+K^2);
a2=(1-1/Q*K+K^2)/(1+1/Q*K+K^2)
; % boost
else 
    b0=(1+1/(Q)*K+K^2)/(1+1/(V0*Q)*K+K^2);
b1=2*(K^2-1)/(1+1/(V0*Q)*K+K^2);
b2=(1-1/Q*K+K^2)/(1+1/(V0*Q)*K+K^2);
a1=2*(K^2-1)/(1+1/(V0*Q)*K+K^2);
a2=(1-1/(V0*Q)*K+K^2)/(1+1/(V0*Q)*K+K^2);
end;

