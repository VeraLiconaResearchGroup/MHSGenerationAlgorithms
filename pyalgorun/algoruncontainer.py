#!/usr/bin/env python

# Python interface to Algorun containers
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

# This file is part of MHSGenerationAlgorithms.

# MHSGenerationAlgorithms is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# MHSGenerationAlgorithms is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

import docker
import requests
import logging
import time
import json

ALGORUN_PORT = 8765
RUN_URL_SUFFIX = "/v1/run"
CONF_URL_SUFFIX = "/v1/config"

class AlgorunError(Exception):
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return repr(self.message)

class AlgorunTimeout(AlgorunError):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr("Timed out after {0} s.".format(self.value))

class AlgorunContainer:
    """
    Container wrapping and implementing an algorithm

    Keyword arguments:
    container_name -- an Algorun container to wrap
    algorithm_name -- a human-readable name for the algorithm
    docker_base_url -- base URL for the Docker client (optional)
    """
    def __init__(self, container_name, algorithm_name = None, base_config = None, docker_base_url = None):
        if algorithm_name is not None:
            alg_name = algorithm_name
        else:
            alg_name = container_name

        # Store configuration variables
        self._name = alg_name
        self._container_name = container_name
        self._base_config = base_config
        self._docker_base_url = docker_base_url

        # Start the container
        self.start()

    def __del__(self):
        try:
            self.stop()
            self.remove()
        except docker.errors.NotFound:
            pass

    def start(self):
        """
        Start the underlying Docker container
        """
        logging.debug("Spawning container {0} from image {1}".format(self._name, self._container_name))
        # Set up Docker client
        docker_client = docker.Client(base_url = self._docker_base_url)

        # Configure port mapping
        port_map = {ALGORUN_PORT: ('',)}
        host_config = docker_client.create_host_config(port_bindings = port_map)

        # Create the container
        try:
            docker_container = docker_client.create_container(image = self._container_name,
                                                              ports = [ALGORUN_PORT],
                                                              host_config = host_config)
        except docker.errors.APIError:
            raise ValueError("Invalid image name {0}".format(self._container_name))

        # Start the container
        docker_client.start(docker_container)

        # Get the port and address for accessing the API.
        # For some reason, .port() returns a list of dicts, so we have to
        # play some games with indices.
        local_port = docker_client.port(docker_container, ALGORUN_PORT)[0]["HostPort"]
        container_network_address = docker_client.port(docker_container, ALGORUN_PORT)[0]["HostIp"]
        if container_network_address == '0.0.0.0':
            container_network_address = 'localhost'

        # Store the container and port as attributes
        self._docker_container = docker_container
        self._local_port = local_port
        self._container_network_address = container_network_address
        self._api_url_base = "http://" + container_network_address + ":" + local_port

        # Wait for the server to spin up to avoid other problems
        while True:
            try:
                # When the container server is up, this will succeed
                requests.get(self._api_url_base)
                break
            except requests.exceptions.ConnectionError:
                # If the GET failed, take a breath and try again, unless the container is dead
                if not docker_client.inspect_container(docker_container)['State']['Running']:
                    raise RuntimeError("Container {0} died unexpectedly during startup poll loop".format(container_name))
                time.sleep(1)
                continue

        # Send the base configuration
        if self._base_config is not None:
            self.change_config(self._base_config)

    def stop(self):
        """
        Stop the underlying Docker container
        """
        docker_client = docker.Client(base_url = self._docker_base_url)
        logging.debug("Stopping container {0}".format(self.name()))
        try:
            docker_client.stop(self._docker_container, timeout=2)
            docker_client.wait(self._docker_container)
            docker_client.remove_container(self._docker_container)
        except docker.errors.APIError:
            logging.debug("Failed to stop container {0}".format(self.name()))

    def restart(self):
        """
        Restart the underlying Docker container
        """
        self.stop()
        self.start()

    def remove(self):
        """
        Remove the underlying data store of this container
        """
        docker_client = docker.Client(base_url = self._docker_base_url)
        logging.debug("Removing container {0}".format(self.name()))
        docker_client.remove_container(self._docker_container)

    def run_alg(self, datafilename, timeout = None):
        """
        Run the algorithm on some data

        Keyword arguments:
        datafilename -- filename of the data to send
        """
        logging.debug("Running algorithm {0}".format(self._name))

        # Build the API url
        run_url = self._api_url_base + RUN_URL_SUFFIX

        # Submit the request
        with open(datafilename, 'rb') as datafile:
            try:
                files = {'input': datafile}
                r = requests.post(run_url, files = files, timeout = timeout)
            except requests.exceptions.Timeout:
                raise AlgorunTimeout(timeout)
            except requests.exceptions.ConnectionError:
                raise AlgorunError("Connection error.")
            except:
                logging.critical("Container {0} threw an exception on analysis POST!".format({self._name}))
                raise

        # Check for errors
        r.raise_for_status()

        # Return the result
        return r.content

    def change_config(self, config):
        """
        Change one or more configuration variables of this container.

        WARNING:
        This will change the state of the container!

        Keyword arguments:
        config -- a dict representation of configuration variables to change
        """
        # Build the API url
        conf_url = self._api_url_base + CONF_URL_SUFFIX

        logging.debug("Sending config {0} to algorithm {1} at URL {2}".format(config, self._name, conf_url))

        # Stringify the input
        payload = json.dumps(config)
        headers = {"content-type": "application/json"}

        # Submit the request and check for errors
        try:
            r = requests.post(conf_url, data = payload, headers = headers)
            r.raise_for_status()
        except:
            logging.critical("Container {0} threw an exception on config POST!".format({self._name}))
            raise

        # Return the result
        return r.content

    def interface_url(self):
        """
        Return the URL for the browser interface
        """

        return self._api_url_base

    def name(self):
        """
        Return the algorithm name of this container
        """
        return self._name

    def __str__(self):
        """
        Print a human-readable string representing this object
        """
        return self._name
