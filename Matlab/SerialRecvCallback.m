function SerialRecvCallback(obj, event)
    global i;
    i = i + 1;
    % Notice of latch interrupt from serial fifo.
    fprintf('Latch data(%d)\n', i);
    % Get data.
    str = fread(obj, 1000);
    % Open and append data to text file.
    fid = fopen('gRAWdata.txt', 'a');
    fprintf(fid,'%s', str);
    % Close file.
    fclose(fid);
    ret = Serial_datamanager();
    if(ret == 1)
        fprintf('Serial_datamanager.\n');
    end
end

function s = Serial_datamanager()
    fprintf('Data Manager.\n');
    s = 1;
end

function s = myfun % 
    num = 100;
    disp(sprintf('%d-%d',num,mysum(num)));
end

function s=mysum(num)
    s=sum(1:num);
end