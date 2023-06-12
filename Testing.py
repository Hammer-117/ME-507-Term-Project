from serial import Serial
import time

shitfuck7 = 12345678
shitfuck8 = 123456

hex_shitfuck7 = hex(shitfuck7)[2:]
if len(hex_shitfuck7)>8:
    raise ValueError
hex_shitfuck7 = hex_shitfuck7.zfill(8)

hex_shitfuck8 = hex(shitfuck8)[2:]
if len(hex_shitfuck8)>8:
    raise ValueError
hex_shitfuck8 = hex_shitfuck8.zfill(8)

print(hex_shitfuck8)
print(hex_shitfuck7)

#stupid_fucking_bytes = shitfuck7.to_bytes(4, "little")
#shitty_bytes = shitfuck8.to_bytes(3)

with Serial("COM5",baudrate=115200, timeout=1) as ser:
    #ser.write(hex_shitfuck8.encode())
    #ser.write(hex_shitfuck7.encode())

    #ser.write(shitty_bytes)
    #ser.write(stupid_fucking_bytes)
    time.sleep(2)

    
    while True:
        ser.write(hex_shitfuck8.encode())
        ser.write(hex_shitfuck7.encode())
        ang = ser.readline()
        dist = ser.readline()
        print((ang))
        print((dist))#"""
    """data = "0"
    try:
        while True:
            ser.write(data.encode())
            print(f"sent {data}")
            time.sleep(.25)
            if ser.in_waiting > 0:
                data = ser.readline()
                data = int(data.decode())
                print(f"received {data}")
                data += 1
                data = str(data)
    except KeyboardInterrupt:
        pass"""