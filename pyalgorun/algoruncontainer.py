#!/usr/bin/env python

# Python interface to Algorun containers
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import docker
import requests
import json
import jsonschema
import logging
import time

ALGORUN_PORT = 8765
RUN_URL_SUFFIX = "/do/run"
CONF_URL_SUFFIX = "/do/config"

class AlgorunContainer:
    """
    Container wrapping and implementing an algorithm

    Keyword arguments:
    container_name -- an Algorun container to wrap
    algorithm_name -- a human-readable name for the algorithm
    schema -- the JSON schema that the container's input must conform to (optional)
    docker_client -- a Docker client object (optional)
    """
    def __init__(self, container_name, algorithm_name = None, input_schema = None, docker_client = None):
        if docker_client == None:
            docker_client = docker.Client()

        if algorithm_name is not None:
            alg_name = algorithm_name
        else:
            alg_name = container_name

        logging.debug("Spawning container {0} from image {1}".format(algorithm_name, container_name))

        # Configure port mapping
        port_map = {ALGORUN_PORT: ('',)}
        host_config = docker.utils.create_host_config(port_bindings = port_map)

        # Create the container
        try:
            container = docker_client.create_container(image = container_name,
                                                       ports = [ALGORUN_PORT],
                                                       host_config = host_config)
        except NotFound:
            raise ValueError("Invalid image name {0}".format(container_name))

        # Start the container
        docker_client.start(container)

        # Get the local port for accessing the API.
        # For some reason, .port() returns a list of dicts, so we have to
        # play some games with indices.
        local_port = docker_client.port(container, ALGORUN_PORT)[0]["HostPort"]

        # Store the container and port as attributes
        self._name = alg_name
        self._container_name = container_name
        self._input_schema = input_schema
        self._container = container
        self._client = docker_client
        self._local_port = local_port
        self._api_url_base = "http://localhost:" + local_port #TODO: Support remote clients

        # Wait for the server to spin up to avoid other problems
        while True:
            try:
                # When the container server is up, this will succeed
                requests.get(self._api_url_base)
                break
            except requests.exceptions.ConnectionError:
                # If the GET failed, take a breath and try again
                time.sleep(1)
                continue


    def __del__(self):
        # Kill the underlying Docker container when this is destroyed
        self._client.kill(self._container)

    def run_alg(self, data):
        """
        Run the algorithm on some data

        Keyword arguments:
        data -- a JSON representation of the data (suitable for input to :func:`~json.dumps`)
        """
        logging.debug("Running algorithm {0}".format(self._name))

        # Validate the input if appropriate
        # TODO: Should we handle this exception or just let it bubble up?
        if self._input_schema is not None:
            jsonschema.validate(self._input_schema, data)

        # Build the API url
        run_url = self._api_url_base + RUN_URL_SUFFIX

        # Stringify the input
        payload = {"input": json.dumps(data)}
        headers = {"content-type": "application/json"}

        # Submit the request
        r = requests.post(run_url, data = json.dumps(payload), headers = headers)

        # Check for errors
        r.raise_for_status()

        # Return the result
        return r.json()

    def change_config(self, config):
        """
        Change one or more configuration variables of this container.

        WARNING:
        This will change the state of the container!

        Keyword arguments:
        config -- a dict representation of a JSON object storing the configuration variables to change
        """
        # Build the API url
        conf_url = self._api_url_base + CONF_URL_SUFFIX

        logging.debug("Sending config {0} to algorithm {1} at URL {2}".format(config, self._name, conf_url))

        # Stringify the input
        payload = json.dumps(config)
        headers = {"content-type": "application/json"}

        # Submit the request
        r = requests.post(conf_url, data = payload)#, headers = headers)

        # Check for errors
        r.raise_for_status()

        # Return the result
        return r.text

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
