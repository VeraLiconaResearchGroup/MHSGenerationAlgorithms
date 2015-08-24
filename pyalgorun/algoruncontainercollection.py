#!/usr/bin/env python

# Python interface to a collection of Algorun containers
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

from algoruncontainer import AlgorunContainer
import json
import jsonschema
import requests
import multiprocessing
import docker

# Helper function to build a single container
def build_container(container_name, alg_name, conf, docker_base_url):
    container = AlgorunContainer(container_name, alg_name, docker_base_url)
    if conf is not None:
        container.change_config(conf)
    return container

# Helper function to run an algorithm on some data
def result_pair(arg_tuple):
    container, data = arg_tuple
    return (container.name(), container.run_alg(data))

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
    alg_set -- Iterable of algorithms to run, each a dict with key "containerName" and "algName" and optionally "config"
    docker_base_url -- base URL for the Docker client (optional)
    num_threads -- number of jobs to run in parallel (optional)
    """
    def __init__(self, alg_set, docker_base_url = None, num_threads = 1):
        # Set up the containers
        # Note: Docker build doesn't parallelize well, so we do this serially
        containers = (build_container(alg.get("containerName"),
                                            alg.get("algName"),
                                            alg.get("config"),
                                            docker_base_url)
                      for alg in alg_set)

        # Set up the job pool
        pool = multiprocessing.Pool(processes = num_threads)

        # Store the container collection in a member
        self._containers = containers
        self._pool = pool
        self._docker_base_url = docker_base_url

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
        data -- a data object compatible with the underlying containers
        """

        results = self._pool.map(result_pair, ((container, data) for container in self))
        return dict(results)

    def stopall(self):
        """
        Stop all the containers
        """
        # We centralize the Docker client to save time
        docker_client = docker.Client(base_url = self._docker_base_url)
        for container in self._containers:
            docker_client.stop(container._docker_container)

    def close(self):
        """
        Terminate the pool and shut down the containers
        """
        self._pool.close()
        self._pool.join()
        self.stopall()
