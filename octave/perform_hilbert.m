fs=5000;
t=[0:1/fs:3-1/fs];
data = load('-ascii', '../data.txt');
c=chirp(t, 20, 2, 100);
c=data(:, 2)';
s=c.*(1+0.5*cos(2*pi*10*t));
h=hilbert(s);
m=abs(h);
a=diff(unwrap(arg(h)))/2/pi*fs;
plot(m,"",a)
