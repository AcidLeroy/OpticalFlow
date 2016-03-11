FROM victorhcm/opencv:latest
MAINTAINER Cody W. Eilar <Cody.Eilar@gmail.com>

RUN \
        apt-get update && apt-get upgrade -y && \
        apt-get install libncurses5-dev -y 
