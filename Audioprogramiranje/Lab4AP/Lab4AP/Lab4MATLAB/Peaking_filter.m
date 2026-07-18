%pozivanje 'peaking filtra''
clear;clc;close all
%Generirati jednostavni periodički signal f=1JHz i trajanje 5s te zadati
%paramtre ''peaking filtra'  i propustiti kroz njega.
%u drugom programu se provjerajva rezultat s prijensonom funkcijom u
%čitavom području frekvencija.
fs=44100; %frekvencija uzorkovanja
dt=1/fs;
f=1000; %frekvencija signala
t=0:dt:5; w0=2*pi*1000;%kružna frekvencija
x=1*cos(w0*t);
n=size(x);%broj uzoraka u spremniku za obradu
figure(1); %slika signala 1
set(gcf,'Color',[1,1,1]);
plot(t,x,'Linewidth',3)   
ylabel('x(t)','FontSize',22);
xlabel('t[s]','FontSize',22);
set(gca,'fontsize',22);grid on;
%racunanje spektra signala
Y=fft(x);
N=n(2);
a1=(abs(Y(1:N/2)))/(N/2);
f=(0:fs/N:fs*(N/2-1)/N);
figure(2);
set(gcf,'Color',[1,1,1])
stem(f,a1), xlabel('f[Hz]','Fontsize',20),title('Amplitudni spektar ulaznog signala');
set(gca,'fontsize',18);
%parametri filtra
G=10; %u dB pojačanje
fcent=1000; Wc=2*fcent/fs;%centralna frekvencija filtra normalizirana od 0 do 1 (fs/2), definirano Zoelzeru
fb=500; Wb=2*fb/fs; %širina pojasa propuštanja normalizirana od 0 do 1
Q=fcent/fb; %Q faktor dobrote definiran u knjizi kao omjer širine pojasa (-6 dB točke) s lijeve i desne strane maksimuma prijenosne funkcije 
y = peakfilt (x, Wc, Wb, G) ;%propuštanje signala kroz filtar zadan sa state-space jednadžbama
figure(3); %slika signala 2
set(gcf,'Color',[1,1,1]);
plot(t,x,'Linewidth',3)   
ylabel('y(t)','FontSize',22);
xlabel('t[s]','FontSize',22);
set(gca,'fontsize',22);grid on;
%racunanje spektra signala
Y=fft(y);
N=n(2);
a2=(abs(Y(1:N/2)))/(N/2);
f=(0:fs/N:fs*(N/2-1)/N);
figure(4);
set(gcf,'Color',[1,1,1])
stem(f,a2), xlabel('f[Hz]','Fontsize',20),title('Amplitudni spektar izlaznog signala');
set(gca,'fontsize',18);

[b0 b1 b2 a1 a2] = coefpeakfilt (fcent, fb, G,fs);

B=[b0 b1 b2]; A=[1 a1 a2];
%opaziti kako je definirana jednadžba diferencija u MATLAB-u, e zaboraviti
%koeficijent 1 i paziti na predznake (tablice u knjigama-vidjeti
%definicije)

figure(5);
set(gcf,'Color',[1,1,1]);
set(gca,'fontsize',22);grid on;

[Hz,Hp]=zplane(B,A);

[H,f]=freqz(B,A,1024,fs);
ampl=20*log10(abs(H));fazna=angle(H);
figure(6);
set(gcf,'Color',[1,1,1]);
subplot(211),semilogx(f,ampl,'k','Linewidth',3),xlabel('f(Hz)','Fontsize',22);ylabel('abs(H)','Fontsize',22),title('Amplitudna frekvencijska karakteristika','Fontsize',22);
set(gca,'fontsize',22);grid on;
subplot(212),plot(f,fazna,'k','Linewidth',3),xlabel('f(Hz)','Fontsize',22);ylabel('angle(H)','Fontsize',22),title('Fazna frekvencijska karakteristika','Fontsize',22);
set(gca,'fontsize',22);grid on;
%grupno kasnjenje
figure(7);
set(gcf,'Color',[1,1,1]);
[gd,w]=grpdelay(B,A,256);
subplot(121);plot(w,gd,'k'),xlabel('w[s-1]','Fontsize',22),ylabel('(gd(w))','Fontsize',22),title('Grupno kasnjenje','Fontsize',22);
set(gca,'fontsize',22);grid on;
%impulsni odziv
subplot(122);
set(gcf,'Color',[1,1,1]);
impz(B,A,50);
xlabel('t[s]','Fontsize',22),ylabel('tau(t)','Fontsize',22)
title('Impulsni odziv','Fontsize',22);set(gca,'fontsize',22);grid on;















