import json
import paho.mqtt.client as mqtt
import serial
import signal
import sys

running = True

def exit_handler(sig, frame):
    """
    This function will be called when we press Ctrl+C.
    """
    global running
    # Setting the variable to false signals that the program should exit
    running = False


def main():
    """Entrypoint"""
    tty_path = "/dev/ttyACM0" # Change this to the port from the Arduino IDE
    if len(sys.argv) > 1:
        tty_path = sys.argv[1]
    
    print(f"Connecting to serial port at {tty_path}")

    signal.signal(signal.SIGINT, exit_handler)

    mqtt_broker_ip = "127.0.0.1"
    client = mqtt.Client()
    client.connect(mqtt_broker_ip, 1883)
    client.loop_start()

    serial_port = serial.Serial(tty_path, 115200, timeout=1)
    serial_port.reset_input_buffer()

    while running:
        next_line = serial_port.readline()
        if next_line:
            next_line = next_line.decode().rstrip()
            print(next_line)
            if next_line.startswith("data: "):
                try:
                    temperature = float(next_line[6:].rstrip())
                    name = "ssnover"
                    client.publish(f"/temperature/{name}", json.dumps({"temperature": temperature}))
                except ValueError:
                    print(f"error: Could not extract data from {next_line}")

    client.loop_stop()
    print("Exiting...")


main()
