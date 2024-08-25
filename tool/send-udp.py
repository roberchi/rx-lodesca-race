import socket


# get IP and port from the ESP32 from args
import sys
if len(sys.argv) != 3:
    print("Usage: python3 send-udp.py <IP> <PORT>")
    sys.exit(1)
# Server IP address and port
server_ip = sys.argv[1]
server_port = int(sys.argv[2])


# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    # Get the data from the user speed and/or steer
    speed = input("Speed: ")
    steer = input("Steer: ")

    # Data to send
    message = f"steer={steer}&speed={speed}"

    # Send the message to the server
    sock.sendto(message.encode(), (server_ip, server_port))
