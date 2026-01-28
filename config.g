; Duet 3 Configuration File
; THIS FILE IS A REFERENCE, NOT FUNCTIONAL AT ALL
; It is recommended to replace the contents of this file with a preset from https://configtool.reprapfirmware.org
G90            ; absolute coordinates
M83            ; relative extruder moves
M550 P"c74rpi" ; set machine label
M669 K1 ; configure CoreXY kinematics

M569 P0.0 S0 D2 ; driver 0.0 goes forwards (X axis)
M569 P0.1 S0 D2 ; driver 0.1 goes forwards (Y axis)

;M569 P0.0 S0 D3 V2000 ; driver 0.0 goes backwards (X axis)
;M569 P0.1 S0 D3 V2000 ; driver 0.1 goes backwards (Y axis)

; NOTE: This is later generated in the Axes and/or Extruders section
M906 X800 Y800  ; set motor driver currents

M906 I30 ; set motor current idle factor
M84 S30  ; set motor current idle timeout

M584 X0.0 Y0.1          ; set axis mapping
M350 X16 Y16 Z16 I1         ; configure microstepping with interpolation
; NOTE: See Smart Drivers section for motor currents
M92 X53.33 Y53.33       ; configure steps per mm
M208 X0:260 Y0:260      ; set minimum and maximum axis limits
M566 X1000 Y1000        ; set maximum instantaneous speed changes (mm/min)
M203 X8000 Y8000        ; set maximum speeds (mm/min)
M201 X6000 Y6000        ; set accelerations (mm/s^2)

M574 X2 P"io0.in" S1 ; configure X axis endstop on high end
M574 Y1 P"io1.in" S1 ; configure Y axis endstop on low end

G92 Z0 ; sets Z to be 0 since Z doesn't exist

M950 F0 C"out5"         ; create fan #0
M106 P0 S0.25 L0 X1 B0.1 ; configure fan #0
