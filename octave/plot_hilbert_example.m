% Load the data from the file (replace 'data.txt' with your file name)
data = load('-ascii', '../data.txt');

% Extract the columns
x = data(:, 1);  % First column is the abscissa
y1 = data(:, 2); % Second column is the first dataset
y2 = data(:, 3); % Third column is the second dataset
y3 = data(:, 4); % Fourth column is the third dataset
y4 = data(:, 5); % Fourth column is the third dataset

% Plot the data
plot(x, y1, 'r-', 'DisplayName', 'data'); % Red line for dataset 1
hold on;
plot(x, y2, 'g-', 'DisplayName', 'amp'); % Green line for dataset 2
plot(x, y3, 'y-', 'DisplayName', 'phase'); % Yellow line for dataset 3
plot(x, y4, 'b-', 'DisplayName', 'freq'); % Blue line for dataset 4

% Add labels, legend, and title
xlabel('Abscissa');
ylabel('Data Values');
title('Plot of Data');
legend show;

% Optionally, grid and hold off
grid on;
hold off;

