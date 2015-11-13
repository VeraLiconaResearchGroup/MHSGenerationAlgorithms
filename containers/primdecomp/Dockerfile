# Set up variables
FROM algorun/algorun:latest
MAINTAINER Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

# Add any algorithm dependencies here
COPY ./macaulay.list /etc/apt/sources.list.d/
RUN apt-key adv --keyserver hkp://keys.gnupg.net --recv-key CD9C0E09B0C780943A1AD85553F8BD99F40DCB31

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    macaulay2 \
    python-dev \
    python-pip \
    && rm -rf /var/lib/apt/lists/

RUN pip install \
    simplejson

# Fetch and build the code
COPY ./src /home/algorithm/src/
WORKDIR /home/algorithm/src

# Add the Algorun manifest and examples
COPY ./algorun_info /home/algorithm/web/algorun_info/

# Add your command to execute an algorithm as environment variable EXEC_COMMAND
ENV EXEC_COMMAND "mhs"
