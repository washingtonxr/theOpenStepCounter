%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Author: Washington Ruan
% Version: 0.1
% Note: Serial Communication Interface.
% Bug: Unkonwn
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;                    % Clear screen.
clear all;              % Clear all data in chche.
delete(instrfindall);   % Really import for initializing.

global i;
global j;
i = 0;
j = 0;
try
    SerialCom = serial('com30');
    fprintf('Serial register successfully.\n');
catch
    error('Cannot open serial');
end

% Set serial com port's parameters.
set(SerialCom, 'InputBufferSize', 1024000);
set(SerialCom, 'BytesAvailableFcnMode', 'byte');
set(SerialCom, 'BytesAvailableFcnCount', 100);
set(SerialCom, 'BaudRate', 115200)
% Set interrupt Callback function.
SerialCom.BytesAvailableFcn = @SerialRecvCallback;

% Try to open SerialCom port if it is available.
try
    fopen(SerialCom);
    fprintf('Serial Port open successfully.\n');
catch
    error('Failed to open serial');
end
