function varargout = serial1(varargin)
% SERIAL1 M-file for serial1.fig
%      SERIAL1, by itself, creates a new SERIAL1 or raises the existing
%      singleton*.
%
%      H = SERIAL1 returns the handle to a new SERIAL1 or the handle to
%      the existing singleton*.
%
%      SERIAL1('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SERIAL1.M with the given input arguments.
%
%      SERIAL1('Property','Value',...) creates a new SERIAL1 or raises the
%      existing singleton*. Starting from the left, property value pairs are
%      applied to the GUI before serial1_OpeningFcn gets called. An
%      unrecognized property name or invalid value makes property application
%      stop. All inputs are passed to serial1_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu. Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES
% Edit the above text to modify the response to help serial1
% Last Modified by GUIDE v2.5 18-Sep-2009 13:30:08
% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',mfilename, ...
                    'gui_Singleton', gui_Singleton, ...
                    'gui_OpeningFcn', @serial1_OpeningFcn, ...
                    'gui_OutputFcn', @serial1_OutputFcn, ...
                    'gui_LayoutFcn', [] , ...
                    'gui_Callback',[]);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end
if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT
%global s selected_com selected_baud selected_databits selected_stopbits selected_parity selected_flowctrl
% --- Executes just before serial1 is made visible.
function serial1_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to serial1 (see VARARGIN)
global s;
handles.mycom = s;
% Choose default command line output for serial1
handles.output = hObject;
% Update handles structure
guidata(hObject, handles);
% UIWAIT makes serial1 wait for user response (see UIRESUME)
% uiwait(handles.figure1);

% --- Outputs from this function are returned to the command line.
function varargout = serial1_OutputFcn(hObject, eventdata, handles)
% varargout cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Get default command line output from handles structure
varargout{1} = handles.output;

% --- Executes on selection change in listbox1.
function listbox1_Callback(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Hints: contents = get(hObject,'String') returns listbox1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from listbox1

% --- Executes during object creation, after setting all properties.
function listbox1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in pushbutton_start.
function pushbutton_start_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_start (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global voltage;
power = 0;
fopen(handles.mycom);
[data,count] = fread(handles.mycom);
fclose(handles.mycom);
for i=1:1:(count/2)
    data_12bits(i) = data(2*i)*256+data(2*i-1);
    voltage(i) = data_12bits(i)*5/4095;
    time(i) = 0.05*i;
    power = power + (voltage(i)^2)*(time(i)*0.001)/900;
end
set(findobj(gcf,'Tag','edit_power'),'String',num2str(power));
set(handles.edit_voltage,'String',num2str(max(voltage)));
plot(handles.axes_waveform,time,voltage);
set(handles.text_time,'Visible','on');
set(handles.text_voltage,'Visible','on');
set(handles.pushbutton_clearfig,'Enable','on');
delete handles.mycom;
% --- Executes on selection change in popupmenu_com.
function popupmenu_com_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_com (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Hints: contents = get(hObject,'String') returns popupmenu_com contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_com
val = get(hObject,'Value');
switch val
    case 1
        s = serial('COM1');
    case 2
        s = serial('COM2');
    case 3
        s = serial('COM3');
    case 4
        s = serial('COM4');
    case 5
        s = serial('COM5');
    case 6
        s = serial('COM6');
end
set(s,'baudrate',57600,'databits',8,'stopbits',1,'parity','none','flowcontrol','none','InputBufferSize',4000);
set(handles.pushbutton_start,'Enable','On');
handles.mycom = s;
guidata(hObject, handles);
% --- Executes during object creation, after setting all properties.
function popupmenu_com_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_com (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- If Enable == 'on', executes on mouse press in 5 pixel border.
% --- Otherwise, executes on mouse press in 5 pixel border or over popupmenu_com.
function popupmenu_com_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_com (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% --------------------------------------------------------------------
function file_Callback(hObject, eventdata, handles)
% hObject    handle to file (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% --------------------------------------------------------------------
function image_Callback(hObject, eventdata, handles)
% hObject    handle to image (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% --------------------------------------------------------------------
function exit_Callback(hObject, eventdata, handles)
% hObject    handle to exit (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close('serial_wave');

% --------------------------------------------------------------------
function open_image_Callback(hObject, eventdata, handles)
% hObject    handle to open_image (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[NameArchivo,PathArchivo]=uigetfile({'*.bmp;*.jpg;*.gif;*png;*.tif;*.ico'});
if ~isequal(NameArchivo,0)
    Archivo=[PathArchivo,NameArchivo];
    Image_Buffer=double(imread(Archivo));
    axes(handles.axes_waveform);
    imshow(uint8(Image_Buffer))
end

% --------------------------------------------------------------------
function save_image_Callback(hObject, eventdata, handles)
% hObject    handle to save_image (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[Filename Pathname]=uiputfile({
	'*.bmp','BMP Files(*.bmp)';'*.jpg','JPG Files(*.jpg)'},'choose a File');
if isequal(Filename,0) || isequal(Pathname,0)
	return;
else
	fpath=fullfile(Pathname, Filename);
end
%F = getframe(h) gets a frame from the figure or axes identified by handle
%h.
h=getframe(handles.axes_waveform);
imwrite(h.cdata,fpath);

% --------------------------------------------------------------------
function import_data_Callback(hObject, eventdata, handles)
% hObject    handle to import_data (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global h;
power = 0;
[NameArchivo,PathArchivo]=uigetfile({'*.txt'});
if ~isequal(NameArchivo,0)
    Archivo=[PathArchivo,NameArchivo];
    Data_Buffer=dlmread(Archivo,'\t');
    len = length(Data_Buffer);
    for i=1:1:len
        time(i) = 0.05*i;
        power = power + (Data_Buffer(i)^2)*(time(i)*0.001)/900;
    end
    set(findobj(gcf,'Tag','edit_power'),'String',num2str(power));
    set(handles.edit_voltage,'String',num2str(max(Data_Buffer)));
    h = plot(handles.axes_waveform,time,Data_Buffer);
    set(handles.text_time,'Visible','on');
    set(handles.text_voltage,'Visible','on');
    set(handles.pushbutton_clearfig,'Enable','on');
end
% --------------------------------------------------------------------
function export_data_Callback(hObject, eventdata, handles)
% hObject    handle to export_data (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global voltage;
[FileName PathName]=uiputfile({
    '*.txt','Txt Files(*.txt)';'*.*','All Files(*.*)'},'choose a File');
str= [PathName FileName];
dlmwrite(str, voltage,'delimiter','\t');

function edit_power_Callback(hObject, eventdata, handles)
% hObject    handle to edit_power (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Hints: get(hObject,'String') returns contents of edit_power as text
%        str2double(get(hObject,'String')) returns contents of edit_power as a double

% --- Executes during object creation, after setting all properties.
function edit_power_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_power (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
	set(hObject,'BackgroundColor','white');
end

function edit_voltage_Callback(hObject, eventdata, handles)
% hObject    handle to edit_voltage (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Hints: get(hObject,'String') returns contents of edit_voltage as text
%        str2double(get(hObject,'String')) returns contents of edit_voltage as a double

% --- Executes during object creation, after setting all properties.
function edit_voltage_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_voltage (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
	set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in pushbutton_clearfig.
function pushbutton_clearfig_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_clearfig (see GCBO)
% eventdata reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
cla;
set(handles.edit_power,'String','');
set(handles.edit_voltage,'String','');
set(handles.text_time,'Visible','off');
set(handles.text_voltage,'Visible','off');
