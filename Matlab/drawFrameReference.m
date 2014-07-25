function drawFrameReference(R,n)

if (nargin > 1)
    hold on
    handle = plot3([R(1,1) + n; n],[R(2,1) + n;n],[R(3,1) + n;n], ...
      [R(1,2) + n;n],[R(2,2) + n;n],[R(3,2) + n; n], ...
      [R(1,3) + n;n],[R(2,3) + n;n],[R(3,3) + n;n]);
else
handle = plot3([R(1,1);0],[R(2,1);0],[R(3,1);0], ...
      [R(1,2);0],[R(2,2);0],[R(3,2);0], ...
      [R(1,3);0],[R(2,3);0],[R(3,3);0]);
end

  
set(handle,'LineWidth', 3)
color = rand(1,3);
color = max([color;0.5,0.5,0.5]);
set(handle(1),'Color',color);
set(handle(2),'Color',color/2);
set(handle(3),'Color',color/8);


grid on
campos([2,1,2]);
axis equal
hold off

figure(gcf)
end