%FIR_jednostavni
clear;
clc;
close all;

%x1 = [0 0 0 0 0 1 1 1 1 1 1 1];
x1 = [0 0 0 0 0 1 0 0 0 0 0 0];
 n=0:1:length(x1)-1;
 %x1=cos(pi/10*n);
 %x1=(-1).^(n+1);
figure(1)
set(gcf,'Color',[1,1,1])
stem(n, x1,'Linewidth',5); % define the unit step sequence
grid on;
set(gca,'fontsize',20);title('x[n]');
xlabel('n')
ylabel('x[n]');
A=[1];
B=[1/3 1/3 1/3];
Fs=44100;
tic
y=filter(B,A,x1);
toc
figure(2)
Ny=length(x1)+length(B)-2;%trajanje odziva ili length(y) ako se n eželite zezati s duljinom trajanja
Ny=length(y);
ny=1:1:Ny;
set(gcf,'Color',[1,1,1])
stem(ny,y,'Linewidth',5);
xlabel('n')
ylabel('y[n]');
set(gca,'fontsize',20);title('y[n]');
grid on;
figure (3)
set(gcf,'Color',[1,1,1])
[Hz,Hp]=zplane(B,A);

[H,w]=freqz(B,A,256);
ampl=abs(H);fazna=angle(H);
figure(4);
set(gcf,'Color',[1,1,1]);
subplot(211),plot(w,ampl,'k','Linewidth',3),xlabel('\omega','Fontsize',22);ylabel('abs(H)','Fontsize',22),title('Amplitudna frekvencijska karakteristika','Fontsize',22);
set(gca,'fontsize',22);grid on;
subplot(212),plot(w,fazna,'k','Linewidth',3),xlabel('\omega','Fontsize',22);ylabel('angle(H)','Fontsize',22),title('Fazna frekvencijska karakteristika','Fontsize',22);
set(gca,'fontsize',22);grid on;
%grupno kasnjenje
figure(5);
set(gcf,'Color',[1,1,1]);
[gd,f]=grpdelay(B,A,1024,Fs);
subplot(121);plot(f,gd,'k'),xlabel('f[Hz]','Fontsize',22),ylabel('(gd(f))','Fontsize',22),title('Grupno kasnjenje','Fontsize',22);
set(gca,'fontsize',22);grid on;
%impulsni odziv
subplot(122);
set(gcf,'Color',[1,1,1]);
impz(B,A,500,Fs);
xlabel('t[s]','Fontsize',22),ylabel('h(t)','Fontsize',22)
title('Impulsni odziv','Fontsize',22);set(gca,'fontsize',22);grid on;
[H,f]=freqz(B,A,256,Fs);
ampl=abs(H);fazna=angle(H);
figure(10);
set(gcf,'Color',[1,1,1]);
subplot(211),plot(f,ampl,'k','Linewidth',3),xlabel('f(Hz)','Fontsize',22);ylabel('abs(H)','Fontsize',22),title('Amplitudna frekvencijska karakteristika','Fontsize',22);
set(gca,'fontsize',22);grid on;
subplot(212),plot(f,fazna,'k','Linewidth',3),xlabel('f(Hz)','Fontsize',22);ylabel('angle(H)','Fontsize',22),title('Fazna frekvencijska karakteristika','Fontsize',22);
set(gca,'fontsize',22);grid on;