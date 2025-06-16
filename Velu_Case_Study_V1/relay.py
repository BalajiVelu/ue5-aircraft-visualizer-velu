import socket
import struct
import time

# Define the struct format string
# 13 doubles 
# '<' = little-endian, 'd' = double
aircraft_state_format = '<' + 'd' * 13
aircraft_state_size = struct.calcsize(aircraft_state_format)

# Open binary file
try:
    file = open("acstate_recording.bin", "rb")
except IOError:
    print("Couldn't open recording file")
    exit(1)

# Setup UDP socket
udp_ip = "127.0.0.1"
udp_port = 6506
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# 50 Hz = 20 ms
period = 0.020
next_message = time.time()

while True:
    data = file.read(aircraft_state_size)
    if len(data) != aircraft_state_size:
        print("Finished the file!")
        break

    # Unpack binary data
    unpacked = struct.unpack(aircraft_state_format, data)

    # For clarity
    (
        sim_time,
        start_lat, start_lon, start_alt,
        lat, lon, alt,
        phi, theta, psi,
        gimbal_phi, gimbal_theta, gimbal_psi
    ) = unpacked

    print(f"{sim_time:.2f}|{lat:.2f}|{lon:.2f}|{alt:.2f}|"
          f"{phi:.2f}|{theta:.2f}|{psi:.2f}|"
          f"{gimbal_phi:.2f}|{gimbal_theta:.2f}|{gimbal_psi:.2f}|")

    # Send the raw data as-is over UDP
    sock.sendto(data, (udp_ip, udp_port))

    # Sleep until next send time
    next_message += period
    sleep_duration = next_message - time.time()
    if sleep_duration > 0:
        time.sleep(sleep_duration)
    else:
        next_message = time.time()  # Avoid drift if we're lagging

file.close()
