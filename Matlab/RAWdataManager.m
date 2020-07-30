close all;
clc;
clear;
filename = 'gdata_test8.txt';
% Mapy three axis's data to X,Y,Z data array.
[x,y,z] = textread(filename,'%f%f%f','delimiter', ' ');
