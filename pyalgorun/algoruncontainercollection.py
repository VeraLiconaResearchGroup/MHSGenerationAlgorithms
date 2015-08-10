#!/usr/bin/env python

# Python interface to a collection of Algorun containers
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

from algoruncontainer import AlgorunContainer
import json
import jsonschema
import requests

# Define the schema for an input object
ALG_SET_SCHEMA = {
    "definitions": {
        "container": {
            "type": "object",
            "properties": {
                "algName": {"type": "string"},
                "containerName": {"type": "string"},
                "config": {"type": "object"}
            },
            "required": ["name", "container"]
        }
    },
    "$schema": "http://json-schema.org/schema#",
    "type": "object",
    "properties": {
        "containers": {
            "type": "array",
            "items": {
                "$ref": "#/definitions/container"
            }
        }
    },
    "required": ["containers"]
}

class AlgorunContainerCollection:
    """
    A collection of Algorun containers for shared benchmarking

    Its primary input is a dict with a single key "containers"
    whose value is a list representing the containers to be run.
    Each container is represented by an object with key "name" giving its name,
    "container" giving its Docker container tag, and optionally "config"
    giving a set of configuration options.

    Containers can be retrieved using index notation.
    In addition, iteration over this class returns the underlying `AlgorunContainer`s.

    Keyword arguments:
    alg_set -- the collection of algorithms to run
    input_schema -- the input schema for these algorithms (optional)
    docker_client -- a Docker client object to host the containers (optional)
    """
    def __init__(self, alg_set, input_schema = None, docker_client = None):
        # Make sure the input matches the schema
        jsonschema.validate(ALG_SET_SCHEMA, alg_set)

        # Set up the containers
        containers = []
        algs = alg_set["containers"]
        for alg in algs:
            container_name = alg["containerName"]
            alg_name = alg["algName"]
            container = AlgorunContainer(container_name, alg_name, input_schema, docker_client)
            try:
                container.change_config(alg["config"])
            except KeyError:
                pass

            containers.append(container)

        # Store the container collection in a member
        self._containers = containers
        self._input_schema = input_schema

    def __getitem__(self, alg_name):
        """
        Retrieve the container with a given name

        Keyword arguments:
        alg_name -- the containerName associated with this container
        """

        try:
            return next(container for container in self._containers if container._name == alg_name)
        except StopIteration:
            raise ValueError("No container named {0}".format(alg_name))

    def __iter__(self):
        return iter(self._containers)

    def __str__(self):
        """
        Print a human-readable string representing this object
        """
        return "Algorun container collection: " + str([str(container) for container in self])

    def run_all_with_input(self, data):
        """
        Run all containers in the collection with the given input data

        Returns a dict mapping the `algName` attribute of each container to its output

        Keyword arguments:
        data -- a JSON data object compatible with the underlying containers
        """

        results = {}

        for container in self:
            result = container.run_alg(data)
            results[container._name] = result

        return results
