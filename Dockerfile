FROM ubuntu:14.04
MAINTAINER Cody W. Eilar <Cody.Eilar@gmail.com>

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get upgrade -y
RUN apt-get install libncurses5-dev -y
RUN apt-get install libv4l-0 -y
RUN apt-get install libavcodec54 -y
RUN apt-get install libavformat54 -y
RUN apt-get install libswscale2 -y
RUN apt-get install libtiff5 -y
RUN apt-get install libjasper1 -y
RUN apt-get install python-pip -y
RUN apt-get install wget -y
RUN pip install pip --upgrade
RUN pip install awscli boto3 argparse
RUN wget https://s3-us-west-2.amazonaws.com/builtopencv/OpenCV-3.1.0-x86_64.tar.gz
RUN tar --strip-components 1 -xvf OpenCV-3.1.0-x86_64.tar.gz -C /usr/local
RUN ldconfig

CMD process_video_list.py --queue_name video_list
