% Anonymous functions
Rot_x = @(x)([1,0,0;0,cosd(x), -sind(x);0,sind(x),cosd(x)]);
Rot_y = @(y)([cosd(y),0,sind(y);0,1, 0; -sind(y), 0, cosd(y)]);
Rot_z = @(z)([cosd(z),-sind(z),0;sind(z), cosd(z), 0; 0, 0, 1]);
Rot = {Rot_x,Rot_y,Rot_z};

R = eye(3);
A = Rot_z(45);

%%

while true
    
    R = Rot_x(360*rand(1))*Rot_y(360*rand(1))*Rot_z(360*rand(1))
    drawFrameReference(R)
    drawFrameReference(C,1)
    T = R_match_axis(R,C)
    Cn = T*C
    drawFrameReference(Cn,1)
    pause()
end