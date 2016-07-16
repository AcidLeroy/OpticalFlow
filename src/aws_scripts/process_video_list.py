#!/usr/bin/env python

from __future__ import print_function
import argparse
import boto3
import botocore
import uuid
import subprocess
import os

def DownloadVideo(path):
    """
    Download video file from s3 and return the local filename in temp
    :param path: s3 path: bucketname/location/to/file.mov
    :return: filename, i.e. /tmp/file.mov
    """
    bucket = path.split('/')[0]
    loc = '/'.join([str(x) for x in path.split('/')[1:]])
    filename = '/tmp/' + path.split('/')[-1]
    s3 = boto3.client('s3')
    s3.download_file(bucket, loc, filename)
    return filename

def DeleteLocalFile(local_file):
    """
    Delete the local downloaded version once it has been processed.
    :param local_file: path to the local file
    :return: None
    """
    os.remove(local_file)

def WriteResults(results, s3_output):
    """
    Write the results to an s3 bucket
    :param results: (string) Feature vector output
    :param s3_output: (string) folder to store the output
    :return:
    """
    # Write the results to s3
    default_output_dir="featureoutput"
    s3_client = boto3.client('s3')
    filename =  str(uuid.uuid1()) + '.csv'
    path_to_file = '/tmp/'+filename
    with open(path_to_file, 'w') as f:
        f.write(results)

    file_path = s3_output + '/' + filename
    s3_client.upload_file(path_to_file, default_output_dir, file_path)

def WriteResultsToQueue(results, output_queue_name):
    """
    Send the results of computing vectors to a queue
    :param results: feature vectors computed by extract_features
    :param output_queue_name: The name of the queue for which to place the results
    :return:
    """
    sqs = boto3.resource('sqs')
    queue = sqs.create_queue(QueueName=output_queue_name)
    queue.send_message(MessageBody=results)



def ComputeResults(program, input_video_file, classification):
    cmd = [program,'--video='+input_video_file, '--classification='+classification]
    results = subprocess.check_output(cmd)
    return results



def main():
    parser = argparse.ArgumentParser(description="A utility to process videos on the SQS queue. The idea with "
                                                 " this utility is that whenever the user uploads new videos "
                                                 "to the SQS queue, we can extract the features on an entire "
                                                 "cluster of machines and return the answer to an S3 bucket. "
                                                 "These results can then be downloaded and analyzed.")
    parser.add_argument('--queue_name', type=str, default='default', help='The name of the queue for which to retrieve'
                                                                          ' video paths and classifications. This must'
                                                                          ' match the name that was used when adding'
                                                                          ' videos to the queue. The default is '
                                                                          '\'default\'. ')
    parser.add_argument('--program', type=str, default='extract_features',
                        help='The command to run to extract features. The purpose of this command is to set the where '
                             'the feature extractor command is just in case it is not on the path. This is useful, '
                             'for example if extract_features is in say, /home/user/my_opticalflow/build/bin/extract_features.')

    args = parser.parse_args()
    queue_name = args.queue_name
    program = args.program
    print("Using queue name: ", queue_name)
    print("Using program \'",program,"\' to extract features")

    # Get the SQS messages
    sqs = boto3.resource('sqs')
    queue = sqs.get_queue_by_name(QueueName=queue_name)

    while(True):
        for message in queue.receive_messages(MessageAttributeNames=['Classification','SQSQueue']):
            classification = message.message_attributes.get('Classification').get('StringValue')
            sqs_output = message.message_attributes.get('SQSQueue').get('StringValue')
            path_to_video = message.body
            output_string = "Path = " + message.body + " with classification = " + classification + " Goes to SQS outputqueue named: " + \
                            sqs_output + '\n'
            print(output_string)
            try:

                # Donwload the video file from S3
                print("Downloading video file from S3 bucket...")
                local_video_file = DownloadVideo(path_to_video)
                print("Done!")


                # Run extract features command
                print("Computing results on ", local_video_file, " ...")
                results = ComputeResults(program, local_video_file, classification)
                print("Done!")

                # Write the results to the SQS queue to be processed by the master node.
                print("Writing results to queue \'", sqs_output, "\'...")
                WriteResultsToQueue(results, sqs_output)
                print("Done!")

                # Clean up the instance so we don't run out of disk space.
                print("Deleting local resource \'", local_video_file, "\'...")
                DeleteLocalFile(local_video_file)
                print("Done!")

            except botocore.exceptions.ClientError as e:
                print("Received an error: ", e)
                # Write Error to queue:
                error = "Error: " + str(e) + "s3 link = " + path_to_video
                WriteResultsToQueue(error, sqs_output)

            except subprocess.CalledProcessError as e:
                print("Could not run command: extract_features on file ", local_video_file)
                print(e)
                #Write error to queue
                error = "Error: " + str(e) + "s3 link = "+ path_to_video
                WriteResultsToQueue("Error: " + error, sqs_output)

            message.delete()


if __name__=="__main__":
    main()

