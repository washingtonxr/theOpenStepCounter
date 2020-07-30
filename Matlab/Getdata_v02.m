close all;
clc;
clear;
% Read data from log.
%filename = 'gRAWdata2.txt';
filename = 'gRAWdata_0501702.txt';
[x,y,z] = textread(filename,'%f%f%f','delimiter', ' ');
axis_all = [x, y, z];
% Get row and column number.
[row, column]=size(axis_all);
% The number of data in a second.
dn_ps = 42;
% Mean filter.
divide_level = 4;
% The number of data in a second divide by divide_level, dr_ps.
dr_ps = dn_ps/divide_level;
% Timestamp of each value.
t_p = 1/dn_ps;
% Dimension.
axis_dime = 3;
% Gravity default.
g_def = 9.8;
% Tempoary varialbe value.
d_index = 0;
% Append time scale into axis_all.
% Legend of array axis_all: 
% ==============================================
% |  Number  |   1       2       3       4     |
% ----------------------------------------------
% |  Content | axis_x  axis_y  axis_z  time    |
% ==============================================
for i0 = 1:row
    axis_all(i0,4) = t_p*i0;
    axis_all(i0,5) = sqrt(axis_all(i0,1)^2 + axis_all(i0,2)^2 + axis_all(i0,3)^2);
    if (axis_all(i0,3) > 0)
        axis_all(i0,6) = -axis_all(i0,5);
    else
        axis_all(i0,6) = axis_all(i0,5);
    end
    % Angel: Nutation\Precession\Rotation
    % Append Nutation Angle
    axis_all(i0,7) = atan(axis_all(i0,2)/axis_all(i0,1));
    axis_all(i0,8) = axis_all(i0,7)*180/pi;
    % Append Rotation Angle
    axis_all(i0,9) = atan(axis_all(i0,3)/(sqrt(axis_all(i0,2)^2+axis_all(i0,1)^2)));
    axis_all(i0,10) = axis_all(i0,9)*180/pi;
end
% axis_ave(index, average_value_x, average_value_y, average_value_z)
for i1 = 1:divide_level:(row-mod(row,divide_level))
    d_index = d_index + 1;
    % Create time stamp.
    axis_ts(d_index) = d_index*t_p*4;
    % Inset average in three axis.
    axis_ave_x(d_index) = (axis_all(i1, 1) + axis_all(i1 + 1, 1) + axis_all(i1 + 2, 1) + axis_all(i1 + 3, 1))/divide_level;
    axis_ave_y(d_index) = (axis_all(i1, 2) + axis_all(i1 + 1, 2) + axis_all(i1 + 2, 2) + axis_all(i1 + 3, 2))/divide_level;
    axis_ave_z(d_index) = (axis_all(i1, 3) + axis_all(i1 + 1, 3) + axis_all(i1 + 2, 3) + axis_all(i1 + 3, 3))/divide_level;
    % Calulate join diagnoal.
    axis_dia(d_index) = sqrt(axis_ave_x(d_index)^2 + axis_ave_y(d_index)^2 + axis_ave_z(d_index)^2);
    % Minus 1g.
    axis_noe(d_index) = axis_dia(d_index) - g_def;
    % Append another dimension.
    axis_ave(d_index,:) = [d_index, axis_ts(d_index), axis_ave_x(d_index), axis_ave_y(d_index), axis_ave_z(d_index), axis_dia(d_index), axis_noe(d_index)];   
end

% figure(1);
% plot(axis_ave,'DisplayName','AXIS data')
% plot(axis_ave(:,2:end),'DisplayName','axis_ave(:,2:end)');
% figure(2);
figure(1);
figures = stackedplot(axis_ave(:,2),axis_ave(:,3:end));
figures.Title = 'Three axis gravity value VS time';
figures.XLabel = 'Time(s)';
figures.DisplayLabels(1) = {'Gravity(m^2/s)'}; 
figures.DisplayLabels(2) = {'Gravity(m^2/s)'};  
figures.DisplayLabels(3) = {'Gravity(m^2/s)'}; 
figures.DisplayLabels(4) = {'Gravity(m^2/s)'};  
figures.DisplayLabels(5) = {'Gravity(m^2/s)'}; 
figure(2);
%plot(axis_ave(:,2),axis_ave(:,7));
plot(axis_ave(:,3:end),'DisplayName','axis_ave(:,3:end)')

figure
subplot(311)
box on
axis tight
plot(axis_all(:,4),axis_all(:,1));
xlabel('time (s) ');
ylabel('u (m/s)')
set(gcf,'color','w')
legend('sample 1','sample 2','sample 3');

subplot(312)
box on
axis tight
plot(t(1:600),v(1:3,1:600))
xlabel('time (s) ');
ylabel('v (m/s)')
set(gcf,'color','w')

subplot(313)
box on
axis tight
plot(t(1:600),w(1:3,1:600))
xlabel('time (s) ');
ylabel('w (m/s)')
set(gcf,'color','w')
