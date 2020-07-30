fclose(s);
delete(s);
clear all;
fid = fopen('Gexty_data.txt', 'w');
a = [];
fprintf(fid, '%s', a);
fclose(fid);
celar all;