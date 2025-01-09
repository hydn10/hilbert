% Load the data from the file (replace 'data.txt' with your file name)
data = load('-ascii', '../data.txt');

% Extract the columns
x = data(:, 1);  % First column is the abscissa
y1 = data(:, 2); % Second column is the first dataset
y2 = data(:, 3); % Third column is the second dataset
y3 = data(:, 4); % Fourth column is the third dataset
y4 = data(:, 5); % Fourth column is the third dataset
yf = data(:, 6);

% Plot the data
plot(x, y4, 'r-', 'DisplayName', 'yg'); % Red line for dataset 1
hold on;
#plot(x, y2, 'g-', 'DisplayName', 'xu'); % Green line for dataset 2
#plot(x, y3, 'y-', 'DisplayName', 'yg_freq'); % Yellow line for dataset 3
#plot(x, y4, 'b-', 'DisplayName', 'yg'); % Blue line for dataset 4
ax = gca;
yl = get(ax,'ylim');
yt = get(ax,'ytick');
h0 = get(ax,'children');
hold on
[ax,h1,h2] = plotyy(ax,0,0,x,yf);
delete(h1)
set(ax(1),'ycolor',get(h0,'color'),'ylim',yl,'ytick',yt)
#plotyy(x, yf, 'b-', 'DisplayName', 'force');

% Add labels, legend, and title
xlabel('Abscissa');
ylabel('Data Values');
title('Plot of Data');
legend show;

% Optionally, grid and hold off
grid on;
hold off;

