import pygame
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


# Initialize Pygame and the joystick module
pygame.init()
pygame.joystick.init()

# Check for connected joysticks/controllers
joystick_count = pygame.joystick.get_count()
if joystick_count == 0:
    print("No joystick connected.")
else:
    # Initialize the first connected joystick
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    print(f"Joystick name: {joystick.get_name()}")

    # Run the main loop
    running = True
    last_steer = -1
    last_speed = -1
    while running:
        # Process event queue
        pygame.event.pump()

        # Get the state of the left stick vertical for speed
        speed = joystick.get_axis(1)

        # Get the state of right stick horizontal movement for steering
        steer = joystick.get_axis(2)

        
        # convert axis valui from -1 to 1 to 0 to 180 where 90 is 0
        steer = int(90 + steer * 90)
        speed = int(90 + speed * 90)
      
        # wait 100 ms
        pygame.time.wait(100)
      
        # send only if steer or spee is changed
        if(last_steer != steer or last_speed != speed):
            # Data to send
            message = f"steer={steer}&speed={speed}"
            # Print the captured data
            print(f"Seed: {speed}, Steer: {steer}")

            # Send the message to the server
            sock.sendto(message.encode(), (server_ip, server_port))
            last_steer = steer
            last_speed = speed

# Quit Pygame
pygame.quit()
