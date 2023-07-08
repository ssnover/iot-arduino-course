#!/usr/bin/python3
import logging
import paho.mqtt.client as mqtt
import serial
import signal
import sys

name = "ssnover"
app_logger = logging.getLogger("app")
comm_logger = logging.getLogger("comm")
running = True


def exit_handler(sig, frame):
    """
    Called by the system to indicate Ctrl+C pressed
    """
    global running
    running = False


def configure_logging():
    """
    Set up of logging to console with format
    """
    logging.root.setLevel(logging.NOTSET)
    console = logging.StreamHandler()
    console.setLevel(logging.DEBUG)
    console.setFormatter(logging.Formatter('%(asctime)s |%(levelname)-8s| %(name)s: %(message)s'))
    app_logger.addHandler(console)
    app_logger.info("Initialized logger")
    comm_logger.addHandler(console)


def handle_serial_line(mqtt_client, line):
    """
    Parses a line of data from the serial port
    """
    line = line.decode().rstrip()
    # We might actually have multiple lines here because the Arduino sends with
    # no time between
    lines = line.split("\r\n")
    for line in lines:
        if line.startswith("info: "):
            comm_logger.info(f'{line[6:]}')
        elif line.startswith("debug: "):
            comm_logger.debug(f'{line[7:]}')
        elif line.startswith("error: "):
            comm_logger.error(f'{line[7:]}')
        elif line.startswith("data: "):
            temperature = float(line[6:].rstrip())
            comm_logger.info(f'Data: {temperature} deg C')
            mqtt_client.publish(f"{name}/temperature", str(temperature))
        else:
            pass


def main():
    """
    Entrypoint
    """
    tty_path = "/dev/ttyACM0"
    if len(sys.argv) > 1:
        tty_path = sys.argv[1]
    configure_logging()
    app_logger.info(f'Connecting to serial port at {tty_path}')
    signal.signal(signal.SIGINT, exit_handler)

    client = mqtt.Client()
    client.connect("192.168.5.100", 1883)
    client.loop_start()

    serial_port = serial.Serial(tty_path, 115200, timeout=10)
    serial_port.reset_input_buffer()
    while running:
        next_line = serial_port.readline()
        handle_serial_line(client, next_line)

    client.loop_stop()
    app_logger.info(f'Exiting...')


main()