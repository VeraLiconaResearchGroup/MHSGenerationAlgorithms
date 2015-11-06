# Using the containers
The [AlgoRun][] containers provided in the [`containers`](containers/) directory make it easy to run any of the algorithms yourself.
First, you'll need a working copy of [Docker][] on your computer.
Docker is available for all major operating systems; consult the Docker [installation guide](//docs.docker.com/engine/installation/) to get started.

## Input and output format
All of the containers use a standardized JSON input format.
The format is very simple.
It has only one key, `sets`, whose value is a list of lists of positive integers giving the sets to be hit.
For example, to represent the input sets [[1, 2, 5], [2, 3, 4], [1, 3]], you could use the following JSON file:

    {
        "sets": [
            [1, 2, 5],
	        [3, 2, 4],
	        [1, 3]
        ]
    }

The output file will include a copy of `sets` as well as two new keys: `transversals`, which gives the hitting sets in the same format, and `timeTaken`, which gives the running time of the algorithm as a float.

**NOTE**: many of these algorithms assume that the set indices are "packed" into [1, 2, …, n].
For maximum performance, you should pre-process your inputs to avoid unnecessarily large index values.
(For example, if you have five indices, they should be [1, 2, 3, 4, 5], not [7, 4, 55, 19243, 1].)

## Working with individual algorithm containers
Once you have Docker installed, you can download and run individual containers easily.
Consult the [containers `README`](containers/README.md) to decide which container(s) you need.
For each one, you must first download it to your computer using the `docker pull` command.
For example, if you want to use the `compsysmed/agdmhs` container, run the following:

    > docker pull compsysmed/agdmhs:latest

You can also use this command to update a container to the latest version at any time.

Once you have a container on your computer, you can launch it using a single command.
For example, if you want to launch the `compsysmed/agdmhs` container which you pulled earlier, run the following:

    > docker run -P --rm compsysmed/agdmhs:latest

Once the container has launched, you will see the following status message:

    Algorun server listening at http://0.0.0.0:8765 locally. Check port mapping for the visible port number ..

You can then find out the port to connect to the container by running `docker ps`, which will give output similar to the following:

    CONTAINER ID        IMAGE               COMMAND                  CREATED             STATUS              PORTS                     NAMES
    6428a0e8cbbf        compsysmed/agdmhs   "/usr/bin/nodejs /hom"   6 seconds ago       Up 5 seconds        0.0.0.0:32774->8765/tcp   cranky_albattani

The number `32774` is the port where we can access this container in this case.
Point your browser to `http://localhost:32274` to connect to the Web interface of the container.
You can type your input into the text box or upload it as a file, using the format documented above.

It is also possible to interface with the containers using HTTP.
The [AlgoRun][] API is documented on each container's landing page.
See [`benchmark.py`](benchmark.py) for an example of how to do this, using the lightweight [`PyAlgoRun`](pyalgorun/) library we provide.

## Running benchmarks on many containers
If you want to compare the results or performance of many algorithms, running the containers manually will get unwieldy.
We have provided the [`benchmark.py`](benchmark.py) script to make this easier.
The script can run a single input through any number of algorithms in any number of containers, with whatever parameter values interest you.
See the help text of `benchmark.py` for details.

### Sanity-checking results
After a run of `benchmark.py`, you will have many result JSON files representing the outputs of your chosen algorithms.
We provide the [`result_checker.py`](result_checker.py) script to help you sanity-check these results.
It will detect inconsistencies among the algorithms by checking whether, for example, all runs with the same cutoff size yielded the same number of transversals.
See the help text of `result_checker.py` for details.

[docker]: //docker.io "Docker"
[algorun]: //algorun.org "Algorun"
