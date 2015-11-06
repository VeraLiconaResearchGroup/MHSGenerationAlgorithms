#1/bin/sh
for containername in `find * -maxdepth 0 -type d`; do
    echo "Building container: " ${containername}
    docker build -t compsysmed/${containername} ${containername}
    done
