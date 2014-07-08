    %   %%%%%%%%%%%%%%%%%%%%%%%%%
    % %       Boat Geometry       %
    % % %%%%%%%%%%%%%%%%%%%%%%%%% %
    %
    % % %%%%%%%%%%%%%%%%%%%%%%%%%-----
    % % %                       %   A
    % % %                       %   |
    % % %                       %   |
    % % %                       %   |
    % % %                       %   |
    % % %           O           %  69"
    % % %           %           %   |
    % % %   O       %<-12"->O---%   |
    % % %           %       A   %   |
    % % %           %     6"|   %   |
    % % %           %       V   %   V
    % % %%%%%%%%%%%%%%%%%%%%%%%%%-----
    % % |<---------34"--------->|
    % % |                       |

kr = 2;
fw = 4;
C = [1 0 0 0 0 0;0 0 1 0 0 0;0 0 0 0 1 0];

K = [1000 1000 1000 10 10 1 1];


    ts = 0.1;
    
% for i = 1:length(minInput)
%     uSat(i) = saturation([minInput(2),maxInput(2)]);
% end


intom = 0.0254;                     %m/in
lbtokg = 0.453592;                  %lb/kg

global port_servo_y_offset port_servo_x_offset starboard_servo_y_offset starboard_servo_x_offset   

port_servo_y_offset = 11.5*intom;                    %m
starboard_servo_y_offset =  -11.5*intom;                    %m
port_servo_x_offset = -25*intom; %-69*intom/2 + 6*intom;        %m
starboard_servo_x_offset = port_servo_x_offset;                          %m

minInput = [-70; -23; -40; -23];
maxInput = [40; 39; 70; 39];

inputBound = [minInput maxInput];

gain.error_force_x =  1000;
gain.error_force_y = 1000;
gain.error_moment_z = 60000;
gain.thruster_force = 10; 
gain.deviation_equilibrum_servo_angle = 0;
gain.deviation_changeof_servo_angle = 1;

m = 46.27; %80*lbtokg;                      %kg
I = 1/12*m*((2*starboard_servo_y_offset)^2 + (2*port_servo_x_offset)^2); %m^2/kg

global p_gain_x d_gain_x p_gain_y d_gain_y p_gain_theta_boat d_gain_theta_boat

%%% PID Control Gaines
p_gain_x = 100;
d_gain_x = 60;%(4*k1x*m)^.5;

p_gain_y = 100;
d_gain_y = 60;%(4*k1y*m)^.5;

p_gain_theta_boat = 100;
d_gain_theta_boat = 60;%(4*k1m*I)^.5;

pdGain.xp = 100;



pdGain.xd = 60;
pdGain.yp = 100;
pdGain.yd = 60;
pdGain.yawp = 50;
pdGain.yawd = 25;


friX1 = 4;
friX2 = 0.76462;
friY  = 8;
friZ  = 4;

portAngles = linspace(-70*pi/180, 10*pi/180,20)';
starboardAngles = -portAngles;

thursters = [linspace(-23, 39,19)'; 0];

initialValues = [portAngles thursters starboardAngles thursters];