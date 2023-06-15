import requests
import json
import math
from serial import Serial
import time

# we are team 3, testing looks for team 2
# 360 CPR, 1:34 gear ratio = 12240 ticks per wheel rotation
# 4.5in wheels

"""!
@file Computer_web_interface.py
@brief This file contains the code for the computer web interface.

This code runs through the steps using pre-captured data from the get_info function. It uses COM5 to communicate
with the Arduino to send data.
"""

def wheel_calcs():
    cpr = 360
    gear_ratio = 34 #1:34
    wheel_diameter = 4.5 #in
    wheel_spacing = 8.125 #in

    ticks_per_rev = cpr*gear_ratio

    in_per_tick = math.pi*wheel_diameter/ticks_per_rev

    #dist_per_circ = math.pi*wheel_spacing

    rad_per_wheel_rev = wheel_diameter/wheel_spacing

    deg_per_wheel_rev = 180/math.pi*rad_per_wheel_rev

    deg_per_tick = deg_per_wheel_rev/ticks_per_rev

    return in_per_tick, deg_per_tick

def get_info():
    response = requests.get('http://10.144.129.5:5001/info')
    data = response.json()

    balls = data['balls']
    robots = data['robots']

    return balls, robots

def main():
    balls, robots = get_info()
    print('Balls:', balls)
    print('Robots:', robots)

def testing():
    
    rescale = 1.5 #pixels/in

    Balls = [{'color': 'green', 'x': 264, 'y': 218}, {'color': 'green', 'x': 282, 'y': 215}, {'color': 'green', 'x': 204, 'y': 207}, {'color': 'green', 'x': 174, 'y': 194}, {'color': 'green', 'x': 208, 'y': 122}, {'color': 'red', 'x': 238, 'y': 162}, {'color': 'blue', 'x': 229, 'y': 134}, {'color': 'yellow', 'x': 325, 'y': 410}, {'color': 'yellow', 'x': 229, 'y': 163}, {'color': 'yellow', 'x': 127, 'y': 121}, {'color': 'yellow', 'x': 120, 'y': 101}, {'color': 'yellow', 'x': 182, 'y': 84}, {'color': 'yellow', 'x': 118, 'y': 74}, {'color': 'yellow', 'x': 152, 'y': 68}, {'color': 'yellow', 'x': 164, 'y': 64}, {'color': 'yellow', 'x': 121, 'y': 65}]
    Robots = [{'center': [195, 180], 'decoded_info': '2', 'orientation': 161}, {'center': [269, 150], 'decoded_info': '1', 'orientation': -85}]

    """
    1. compile list of balls of correct color
    2. find location of robot
    3. find closest ball
    4. calculate necessary angle and distance
    """

    ballpos = []
    balldist = []
    ballrelpos = []

    for robot in Robots:
        if robot['decoded_info'] == '2':
            robopos = robot['center']
            roboang = robot['orientation']

    for ball in Balls:
        if ball['color'] == wantedcolor:
            ballpos.append((ball['x'], ball['y']))

    for ball in ballpos:
        ballrelpos.append((ball[1]-robopos[0], ball[1]-robopos[1]))
        balldist.append(math.sqrt(ballrelpos[-1][0]**2 + ballrelpos[-1][1]**2))

    mindist = min(balldist)
    mindex = balldist.index(mindist)

    newdir = math.atan(ballrelpos[mindex][1]/ballrelpos[mindex][0])
    newdir = newdir*180/math.pi
    if ballrelpos[mindex][0] <= 0:
        newdir = 180+newdir
    dirdiff = newdir-roboang

    mindist = mindist/rescale

    return mindist, dirdiff

def encoder_encoding(in_per_tick, deg_per_tick, turn_angle, distance):

    tickdist = distance/in_per_tick
    tickdist = int(math.ceil(tickdist))

    turntick = turn_angle/deg_per_tick
    turntick = int(math.ceil(turntick))

    dist_bytes = tickdist.to_bytes(4)
    turn_bytes = turntick.to_bytes(3)

    return turn_bytes, dist_bytes

def sendticks(turn_ticks, straight_ticks):

    with Serial("COM4",115200, timeout=1) as ser:
        ser.write(turn_ticks)
        #ser.write(b",")
        ser.write(straight_ticks)

def sendinch(serobj, turn_deg, straight_in):
    quarter = math.ceil(straight_in*4)
    turn_deg = math.ceil(turn_deg)

    hex_turn = hex(turn_deg)[2:]
    if len(hex_turn)>4:
        raise ValueError
    hex_turn = hex_turn.zfill(4)
    
    hex_dist = hex(quarter)[2:]
    if len(hex_dist)>4:
        raise ValueError
    hex_dist = hex_dist.zfill(4)

    if turn_deg < 0:
        turn_deg += 360

    serobj.write(turn_deg.to_bytes(4))
    serobj.write(quarter.to_bytes(4))
    
    """good = 0
    while good == 0:
        if return_ang == turn_deg and return_dist == quarter:
            good == 1
        else:
            serobj.write(hex_turn)
            serobj.write(hex_dist)
            return_ang = int(serobj.readline())
            return_dist = int(serobj.readline())"""

if __name__ == '__main__':
    #main()
    with Serial("COM5",9600, timeout=1) as ser:
        time.sleep(2)
        while True:
            wantedcolor = 'green'
            #in_tick, deg_tick = wheel_calcs()
            travdist, theta = testing()
            #turn, straight = encoder_encoding(in_tick, deg_tick, theta, travdist)
            sendinch(ser, theta, travdist)

        #Serial.write()