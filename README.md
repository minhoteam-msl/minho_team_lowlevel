# minho_team_lowlevel
### Baudrate 115200
### Timeout 200ms

#Full packet (send) :
Variable | Range
----------  | -------
linear_speed | (0->100)
angular_speed | (-100->100)
direction | (0->360)
kick_type | Binary
kick_strength | (0->25)
enable_drib1 | Binary
vel_drib1 | (0->255)
enable_drib2 | Binary
vel_drib2 | (0->255)
resetenc1 | Binary
resetenc2 | Binary
resetenc3 | Binary

String : linear_speed,angular_speed,direction,kick_type,kick_strength,enable_drib1,vel_drib1,enable_drib2,vel_drib2,resetenc1,resetenc2,resetenc3


#Full packet (receive) :
Variable | Range
----------  | -------
imu_angle | (0->360)
free_wheel | Binary
ball_sensor | Binary
Enc1 | (32,768->32,767)
Enc2 | (32,768->32,767)
Enc3 | (32,768->32,767)
PcBattery | (0.0->13.0)
CamBattery | (0.0->7.0)
HwBattery | (0.0->26.0)

String : imu_angle,free_wheel,ball_sensor,Enc1,Enc2,Enc3,PcBattery,CamBattery,HwBattery

#Commands
* Full packet string (send)
* S<VALUE\> - Move servo to defined value
* p - OMNI 3MD P Component -= 50
* P - OMNI 3MD P Component += 50
* i - OMNI 3MD I Component -= 50
* I - OMNI 3MD I Component += 50
* d - OMNI 3MD D Component -= 50
* D - OMNI 3MD D Component += 50
* b - OMNI 3MD Ramp B Component -= 50
* B - OMNI 3MD Ramp B Component += 50
* n - OMNI 3MD Ramp N Component -= 50
* N - OMNI 3MD Ramp N Component += 50
* m - OMNI 3MD Ramp M Component -= 50
* M - OMNI 3MD Ramp M Component += 50
* \# - Redefine IMU '0' level
* x - Reset encoders' count
* K<VALUE\>  - Kick ball with defined strength (value) 