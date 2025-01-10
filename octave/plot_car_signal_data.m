% Load the data from the file (replace 'data.txt' with your file name)
data = load('-ascii', '../data.txt');

% Extract the columns
t = data(:, 1);
g = data(:, 2);
tf = data(:, 3);
g_freq = data(:, 4);
tf_freq = data(:, 5);
g_phase = data(:, 6);
tf_phase = data(:, 7);
g_amp = data(:, 8);
tf_amp = data(:, 9);

delta_phase = unwrap(g_phase - tf_phase) - 2 * pi;
delta_phase = delta_phase * 180 / pi;

% Plot the data
plot(t, delta_phase, 'b-', 'DisplayName', 'delta_phase'); % Red line for dataset 1
hold on;
#plot(x, y2, 'g-', 'DisplayName', 'xu'); % Green line for dataset 2
#plot(x, y3, 'y-', 'DisplayName', 'yg_freq'); % Yellow line for dataset 3
#plot(x, y4, 'b-', 'DisplayName', 'yg'); % Blue line for dataset 4
ax = gca;
yl = get(ax,'ylim');
yt = get(ax,'ytick');
h0 = get(ax,'children');
hold on
[ax,h1,h2] = plotyy(ax,0,0,t,tf_amp);
delete(h1)
set(ax(1),'ycolor',get(h0,'color'),'ylim',yl,'ytick',yt)

% Add labels, legend, and title
xlabel('Abscissa');
ylabel('Data Values');
title('Plot of Data');
legend show;

% Optionally, grid and hold off
grid on;
hold off;

