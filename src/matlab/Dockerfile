FROM centos:7

Maintainer Cody W. Eilar

RUN yum update -y
RUN yum -y install epel-release && rpm -Uvh http://li.nux.ro/download/nux/dextop/el7/x86_64/nux-dextop-release-0-5.el7.nux.noarch.rpm
RUN yum install gstreamer-0.10* -y
RUN yum install gstreamer-ffmpeg-0.10.13-15.el7.nux.x86_64 -y

