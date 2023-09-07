# Uploading Data to the Database

In this section we'll take our newly measured temperature data and parse it from the messages we're receiving from the Arduino. That data can then be uploaded into a database, in this case an InfluxDB instance. If you're following along remotely, see the appendix section on running InfluxDB with Docker.

We'll be using MQTT as a means of uploading data into InfluxDB. However the database also supports other means including a REST API, but it requires an API token to use. InfluxDB supports plugins for accepting data records via Telegraf, an associated project, and one of those allows subscribing to certain messages on an MQTT broker.

## A Little About MQTT

MQTT is a means of sending messages between programs whereby the programs agree on a topic name like `"temperature_data"` and then the programs can publish data on this topic by sending the data to a broker or it can subscribe to data on this topic, again by talking to the broker.

In our case, the Python program will connect to the MQTT broker and send data to it by publishing to a topic. The InfluxDB/Telegraf instance will connect to the MQTT broker as a subscriber so that the broker will send it data. That data gets put into InfluxDB.

## Using MQTT with Python

We'll be using a third party library from Eclipse called `paho-mqtt` in order to publish data in our python program. It doesn't require much code to get started:

```py
import paho.mqtt.client as mqtt
import serial
import signal
import sys

# --snip--

def main():
    # --snip--
    signal.signal(signal.SIGINT, exit_handler)

    nqtt_broker_ip = "192.168.5.100"
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
                    temperature = float(line[6:].rstrip())
                    name = "ssnover"
                    client.publish(f"{name}/temperature", str(temperature))
                except ValueError:
                    print(f"error: Could not extract data from {next_line}")

    client.loop_stop()
    print("Exiting...")

# --snip--
```

In the above code, we create a new instance of the `Client` class and connect it to an MQTT broker. I've set `mqtt_broker_ip` to the IP of the device on my network that's hosting the broker and port 1883, which is the default port for MQTT. 

The MQTT library allows the user to handle an event loop in order to gracefully handle incoming messages if you're subscribed to any topics. Since we're not doing here, I've called `Client.loop_start()` and `Client.loop_end()` before and after the main loop. This just controls the starting and stopping of an additional thread which does the actual communication with the broker. If you were to leave this out, you'd actually see no data getting sent!

Finally, let's look at actually publishing the data. We know that the format of data messages coming from the Arduino is `"data: [DECIMAL NUMBER]"` so we first check that the line starts with that expected prefix. If it passes that basic check, we take everything after the prefix, remove any dangling whitespace characters with `rstrip()`, and then try to convert that to a float as a sanity check. This can throw an exception of `ValueError` if it's not a valid decimal number so I've just handled that by logging it and moving on. To finally publish, we need the topic name and the data. I use `"ssnover/temperature"` for the topic for the purpose of the in-person session so I suggest using your name in that pattern as well so the database can be ready to read it. The topic data is just a string format so we convert our temperature data back into a string.

If everything is working correctly, you ought to see the temperature data coming in on the dashboard in the InfluxDB instance, go give it a look!