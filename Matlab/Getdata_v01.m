close all;
clc;
clear;
% Read data from log.
filename = 'gRAWdata2.txt';
[x,y,z] = textread(filename,'%f%f%f','delimiter', ' ');
axis_all = [x, y, z];
% The number of data in a second.
dn_ps = 42;
% Mean filter.
divide_level = 4;
% The number of data in a second divide by divide_level, dr_ps.
dr_ps = dn_ps/divide_level;
% Timestamp of each value.
t_p = 1/dn_ps;
% row = 1218;
row = 2792;
% Dimension.
axis_dime = 3;

d_index = 0;
% axis_ave(index, average_value_x, average_value_y, average_value_z)
for i1 = 1:divide_level:row
    d_index = d_index + 1;
    axis_ts(d_index) = d_index*t_p*4;
    axis_ave_x(d_index) = (axis_all(i1, 1) + axis_all(i1 + 1, 1) + axis_all(i1 + 2, 1) + axis_all(i1 + 3, 1))/divide_level;
    axis_ave_y(d_index) = (axis_all(i1, 2) + axis_all(i1 + 1, 2) + axis_all(i1 + 2, 2) + axis_all(i1 + 3, 2))/divide_level;
    axis_ave_z(d_index) = (axis_all(i1, 3) + axis_all(i1 + 1, 3) + axis_all(i1 + 2, 3) + axis_all(i1 + 3, 3))/divide_level;
    axis_dia(d_index) = sqrt(axis_ave_x(d_index)^2 + axis_ave_x(d_index)^2 + axis_ave_x(d_index)^2);
    axis_noe(d_index) = axis_dia(d_index) - 9.8;
    axis_ave(d_index,:) = [d_index, axis_ts(d_index), axis_ave_x(d_index), axis_ave_y(d_index), axis_ave_z(d_index), axis_dia(d_index), axis_noe(d_index)];
end
figure(1);
plot(axis_ave,'DisplayName','AXIS data')
plot(axis_ave(:,2:end),'DisplayName','axis_ave(:,2:end)')
% Append another dimension.

for i = 1:row
    t_s(i) = t_p*i;
    com(i) = sqrt(x(i)^2+y(i)^2+z(i)^2);
    
end

for i = 1:row/devide_level
    t_mean(i) = t_p*i*4;
end

%figure(1);plot(t_s, com);title('Line Plot');xlabel('s');ylabel('m/s');

% figure(2);
% plot(z,'DisplayName','z');hold on;plot(y,'DisplayName','y');plot(x,'DisplayName','x');plot(com,'DisplayName','com');hold off;

for av_idx = 1:row/devide_level
    x_ave(av_idx) = (x(4*av_idx-3)+x(4*av_idx-2)+x(4*av_idx-1)+x(4*av_idx))/4;
    y_ave(av_idx) = (y(4*av_idx-3)+y(4*av_idx-2)+y(4*av_idx-1)+y(4*av_idx))/4;
    z_ave(av_idx) = (z(4*av_idx-3)+z(4*av_idx-2)+z(4*av_idx-1)+z(4*av_idx))/4;
end

for av_idx = 1:(row/devide_level-1)
    if(x_ave(av_idx+1)>x_ave(av_idx))
        x_axis_max(av_idx) = x_ave(av_idx+1);
    else
        x_axis_max(av_idx) = x_ave(av_idx);
    end
    if(y_ave(av_idx+1)>y_ave(av_idx))
        y_axis_max(av_idx) = y_ave(av_idx+1);
    else
        y_axis_max(av_idx) = y_ave(av_idx);
    end
    if(z_ave(av_idx+1)>z_ave(av_idx))
        z_axis_max(av_idx) = z_ave(av_idx+1);
    else
        z_axis_max(av_idx) = z_ave(av_idx);
    end
end

for i = 1:row/devide_level
    axis_ave(i) = sqrt(x_ave(i)^2+y_ave(i)^2+z_ave(i)^2);
end

% axis_max = max(axis_ave);
% axis_min = min(axis_ave);

figure(3);
plot(t_mean, axis_ave);

figure(4);
grid on;
plot(z,'DisplayName','z');hold on;plot(y,'DisplayName','y');plot(x,'DisplayName','x');hold off;

figure(5);
plot(t_mean, x_ave, '-o');
hold on;
plot(t_mean, y_ave, '-x');
hold on;
plot(t_mean, z_ave, '-^');
hold off;grid on;

%plot(t_mean,x_ave);hold on;plot(t_mean,y_ave);hold on;plot(t_mean,z_ave);hold off;
%plot(x_ave,'DisplayName','x_ave');hold on;plot(y_ave,'DisplayName','y_ave');plot(z_ave,'DisplayName','z_ave');hold off;
%plot(FFT(com));

% Acquire FFT.
fs = 4;
N = 128;
n = 0:N-1;
t = n/fs;
f = n*fs/N;

fft_x_ret = fft(x_ave, N);
mag_x = abs(fft_x_ret);
figure(6);
plot(f, mag_x);
xlabel('Frequency/Hz');
ylabel('Amplitude');title('N=128');grid on;

fft_y_ret = fft(y_ave, N);
mag_y = abs(fft_y_ret);
figure(7);
plot(f, mag_y);
xlabel('Frequency/Hz');
ylabel('Amplitude');title('N=128');grid on;

fft_z_ret = fft(z_ave, N);
mag_z = abs(fft_z_ret);
figure(8);
plot(f, mag_z);
xlabel('Frequency/Hz');
ylabel('Amplitude');title('N=128');grid on;

fft_a_ret = fft(axis_ave, N);
mag_a = abs(fft_a_ret);
figure(9);
plot(f, mag_a);
xlabel('Frequency/Hz');
ylabel('Amplitude');title('N=128');grid on;


