# AWS Scripts

In order to distribute the `extract_features` program on the cloud,
code in this directory must be leveraged. In order to
successfully run this code, there are a few prerequisites
that must be first met:

## Prerequisites
- Must have an AWS account
- Must have access to the S3 bucket hosting the AOLME videos
(Please email [Cody Eilar](mailto:cody.eilar@gmail.com) if you wish to have access)
- Must have an ECS cluster setup that pulls from the latest
optical flow docker build located here: [AWS Container Registry](883856218903.dkr.ecr.us-west-2.amazonaws.com/optical-flow:latest).
 - `docker pull 883856218903.dkr.ecr.us-west-2.amazonaws.com/optical-flow:latest`
 - If you have permission denied, be sure you follow
 the steps on the AWS site for performing a docker pull,
 and then you made need to request access from [Cody Eilar](mailto:cody.eilar@gmail.com).
- Once the cluster is properly configured, you will then
be able to run the scripts in this directory.

## Description of Scripts
- `add_videos_to_process.py` This is the script that is used
to add video uris to an SQS queue. The videos are then popped
off the queue by the running docker images and the output
of extract_features is put into another queue, which is then read by this program and stored into a variable.
- `message_type.py` - This contains the layout of the messages
sent to the SQS queue for processing.
- `process_video_list.py` - This is ultimately the script
that does all the processing in the Docker image. It essentially
pulls video URI's off the SQS queue and processes them. When
processing is done, it then puts the answer onto another SQS
queue to be processed by the master node.
- `example_videos_to_process.txt` - This is an example input
into the `add_videos_to_process.py` script. This
illustrates how to send videos to the processing nodes.
