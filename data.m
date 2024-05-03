% Generate random data
datas = randn(1, 320);

% Design FIR filter
order = 28; % Filter order
cutoff_freq = 8/24; % Cutoff frequency
h = fir1(order, cutoff_freq);

% Filter the data
filtered_data = filter(h, 1, datas);

% Print datas
fprintf('Data:\n');
fprintf('%+.10f, ', datas);
fprintf('\n\n');

% Print the filtered data
fprintf('Filtered data:\n');
fprintf('%+.10f, ', filtered_data);
fprintf('\n\n');

% Format data and filtered_data as strings with ", " separator
formatted_datas = arrayfun(@(x) sprintf('%+.10f, ', x), datas(1:end-1), 'UniformOutput', false);
formatted_filtered_data = arrayfun(@(x) sprintf('%+.10f, ', x), filtered_data(1:end-1), 'UniformOutput', false);

% Concatenate formatted data into a single string with spaces after commas
data_str = strjoin(formatted_datas, '');
filtered_data_str = strjoin(formatted_filtered_data, '');

% Add last data without ", "
data_str = [data_str sprintf('%+.10f', datas(end))];
filtered_data_str = [filtered_data_str sprintf('%+.10f', filtered_data(end))];

% Split data_str and filtered_data_str into lines every 8 data points
data_lines = split(data_str, ', ');
filtered_data_lines = split(filtered_data_str, ', ');

% Write data to "data.txt" with line breaks every 8 data points and prepend each data point with "f"
fid_data = fopen('data.txt', 'w');
for i = 1:length(data_lines)
    fprintf(fid_data, '%sf', data_lines{i}); % Prepend 'f' to each data point
    if mod(i, 8) == 0 % Insert newline after every 8 data points
        fprintf(fid_data, '\n');
    else
        fprintf(fid_data, ', ');
    end
end
fclose(fid_data);


% Write filtered data to "filtered_data.txt" with line breaks every 8 data points and prepend each data point with "f"
fid_filtered_data = fopen('filtered_data.txt', 'w');
for i = 1:length(filtered_data_lines)
    fprintf(fid_filtered_data, '%sf', filtered_data_lines{i}); % Prepend 'f' to each data point
    if mod(i, 8) == 0 % Insert newline after every 8 data points
        fprintf(fid_filtered_data, '\n');
    else
        fprintf(fid_filtered_data, ', ');
    end
end
fclose(fid_filtered_data);