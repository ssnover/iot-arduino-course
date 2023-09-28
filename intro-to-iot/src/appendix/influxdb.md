# A2. Running InfluxDB

If you're running through this course asynchronously, you'll need to set up some software on your own computer in order to get to the big pay-off. This chapter talks through the set of software services I'll be running on my computer.

This part I'm less capable of ensuring runs on Windows and MacOS. I'm running this on Ubuntu 22.04 desktop edition using Docker version 224.0.4. You can find instructions for installing `docker` here: https://docs.docker.com/engine/install/ubuntu/

I'll be curt on explanation here and instead point to a repository with a working set of configuration files. The repository hosting them all is here: https://github.com/ssnover/mqtt-influx-config 

You can run it while in the root directory with `docker compose up -d` which will launch three services: an MQTT broker, an Influx timeseries database, and an ingress service for the data that connects the two.

The first time that you run it, you'll be prompted to create an admin user, organization, and bucket on the webpage at `http://localhost:8086`. You'll also need to generate the API token with the UI. Once you've created these, edit the respective fields in the `telegraf.conf` file. If you host this for an extended period, make sure not to commit any files containing the API token. Relaunch the services with `docker compose down` and `docker compose up -d` and the new configuration should take hold.