FROM centos:7

Maintainer Cody W. Eilar

RUN yum -y install epel-release && rpm -Uvh http://li.nux.ro/download/nux/dextop/el7/x86_64/nux-dextop-release-0-5.el7.nux.noarch.rpm
RUN yum update -y
RUN yum install gstreamer0.10* unzip -y
RUN yum install python-pip -y
RUN pip install pip --upgrade
RUN pip install boto3

