# MHS Generation Algorithms
This repository contains a collection of implemented algorithms for generating Minimal Hitting Sets and a framework for running and comparing them.
See the [specifications file](specifications.md) for information about the requirements and behavior of each implementation.

## Containers
Each algorithm is implemented as a [Docker][] container using the [Algorun][] framework.
Each one must be built before it can be run.
To build, for example, the `MHS2` container, navigate to `algorithms/mhs2` and then run

    docker build -t agdphd/mhs2 .

You may replace the name `agdphd/mhs2` with any other name you like.
Once this is done, you can spool up the container by running

    docker run -p 31331:8765 agdphd/mhs2

You may replace the port `31331` with another if you prefer; however, the port `8765` is defined by Algorun and must be left as-is.
Point your web browser to [//localhost:31331] to access the Web interface for the algorithm container.

## PyAlgorun
We also provide a Python package for managing Algorun containers in a convenient, procedural way.
It requires the [jsonschema] Python library, which is available in the Debian `python-jsonschema` package or with `pip install jsonschema`.

## License
Code in this repository is distributed under a mixture of licenses.
See each file for details.
(In particular, code written by AGD for this project is typically released under the Artistic License 2.0, while upstream algorithm code is distributed under a variety of licenses.)

[docker]: //docker.io "Docker"
[algorun]: //algorun.org "Algorun"
[jsonschema]: //pypi.python.org/pypi/jsonschema "Python-jsonschema"
