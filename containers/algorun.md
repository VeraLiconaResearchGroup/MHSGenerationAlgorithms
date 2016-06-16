# Accessing the algorithm containers
Each of the containers in this directory is built using the [AlgoRun](http://algorun.org) framework.
These containers can be used through a browser interface, either locally or on a server hosted by the AlgoRun project.

## Running containers on `algorun.org`
To connect to the container instances hosted by the AlgoRun project, point your browser to the URL for the container listed in [`README.md`](README.md).
For example, the [`agdmhs`](agdmhs/) container can be accessed at [`http://agdmhs.algorun.org`](http://agdmhs.algorun.org).

## Running containers locally
To run a container locally, you must first fetch it to your computer using the `docker pull` command.
For example, to download the `agdmhs` container, run the following in your terminal:

    docker pull compsysmed/agdmhs

This will download the container image and install it on your computer.
To start the container, run the following in your terminal:

    docker run -it -p 32768:8765 compsysmed/agdmhs

Point your browser to [`http://localhost:32768`](http://localhost:32768) to access the container interface.

# Computing with the containers
Once you have the algorithm interface loaded in your browser (either by connecting to `algorun.org` or by running the appropriate container locally), you're ready to run some computations!

## Configuring input sets
To do this, you'll need an input file.
All of the MHS containers use the same JSON-based input format.
Your input set family should be represented as a list of lists of positive integers using the key `"sets"`.
For example, the set family {{1, 2, 5}, {2, 3, 4}, {1, 3}} becomes the following in our input format:

    {
        "sets": [
    	[1, 2, 5],
    	[3, 2, 4],
    	[1, 3]
        ]
    }

You can load this example in the input entry area by clicking the "Load sample data" button.

## Configuring options
Many of the containers have configurable options.
For example, a container may implement more than one algorithm, in which case you must choose which one to run.
Likewise, if an algorithm supports cutoff enumeration, you must specify your desired cutoff.

Click the "Change parameters" button to see the available configurable parameters and their default values.
More information about these options is given in the "About" section of the interface.

## Running the algorithm
Once you've entered your input set family and your parameter choices, you're ready to go!
Press the "Run computation" button to start the calculation.
When it's ready, the result will display in the "OUTPUT" section.

The result will also be presented in JSON format.
There will be three keys:
* `"sets"`, which gives the original input set family;
* `"timeTaken"`, which gives the computation time in seconds as a float; and
* `"transversals"`, which gives the MHSes of the input sets.

For example, running with the input family described above might give the following output:

    {
        "transversals": [
            [
                3,
                5
            ],
            [
                2,
                3
            ],
            [
                1,
                4
            ],
            [
                1,
                3
            ],
            [
                1,
                2
            ]
        ],
        "timeTaken": 0.006952047348022461,
        "sets": [
            [
                1,
                2,
                5
            ],
            [
                3,
                2,
                4
            ],
            [
                1,
                3
            ]
        ]
    }
