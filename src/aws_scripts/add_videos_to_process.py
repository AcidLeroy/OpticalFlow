#!/bin/env python

from __future__ import print_function

import argparse
import os
import pandas as pd
import message_type as mt
import boto3
from time import sleep
import time

def GetMessagesInQueue(sqs_resource, queue_name):
    # Get the queue. This returns an SQS.Queue instance
    queue = sqs_resource.get_queue_by_name(QueueName=queue_name)

    # You can now access identifiers and attributes
    num_messages = queue.attributes.get('ApproximateNumberOfMessages')
    return int(num_messages)

def GetNumberOfOutputs(s3_bucket, key):
    return sum(1 for _ in s3_bucket.objects.filter(Prefix=key))

def DeleteMessagesInBucket(s3_bucket, key):
    for key in s3_bucket.list(prefix=key):
        key.delete()

def DownloadMessagesInBucket(s3_bucket, key):
    s3 = boto3.resource('s3')
    for obj in s3_bucket.objects.filter(Prefix=key):
        s3.Object(s3_bucket.name, obj.key).download_file(obj.key)

def DeleteMessagesInBucket(s3_bucket, key):
    s3 = boto3.resource('s3')
    for obj in s3_bucket.objects.filter(Prefix=key):
        s3.Object(s3_bucket.name, obj.key).delete()




def main():
    parser = argparse.ArgumentParser(description="A utility to add a list of video files that exist on Amazon S3 from which to extract features.")
    parser.add_argument("input_file", type=str, help="Path to a file that contains a list"
                                                     " of S3 files to process. File must be a comma separated listed of path,classification.")
    parser.add_argument('--queue_name', type=str, default='default', help='Name of the queue to use for adding the messages. '
                                                                          'If the queue does not already exist, one is created.'
                                                                          ' The default queue is named \'default\'.')

    args = parser.parse_args()


    input_file = os.path.abspath(args.input_file)
    queue_name = args.queue_name

    print("Processing file = ", input_file)
    print("Using queue = ", queue_name)
    df = pd.read_csv(input_file)
    # Create the messages to send to sqs
    batch_messages = mt.create_messages_from_df(df)

    # Create a queue or retrieve one with the same name
    sqs = boto3.resource('sqs')
    queue = sqs.create_queue(QueueName=queue_name)

    # Send the messages to SQS
    for messages in batch_messages:
        response = queue.send_messages(Entries=messages)
        print("Errors received from response: ", response.get('Failed'))



    # Wait for queue to become empty
    num_messages = GetMessagesInQueue(sqs, queue_name)

    start = time.time()
    zero_counter = 0
    while zero_counter < 3:
        print("There are ", num_messages, " remaining in queue")
        sleep(1)
        num_messages = GetMessagesInQueue(sqs, queue_name)
        if num_messages == 0:
            zero_counter += 1
        else:
            zero_counter = 0

    stop = time.time()
    print("It took ", stop - start, " seconds for all messages to be ingested.")

    # Wait for all output to be available
    s3 = boto3.resource('s3')
    my_bucket = s3.Bucket('featureoutput')
    total_features = len(df)
    key = df['s3_output'][0]
    features_in_bucket = GetNumberOfOutputs(my_bucket, key)
    while features_in_bucket != total_features:
        features_in_bucket = GetNumberOfOutputs(my_bucket, key)
        print("There are ", features_in_bucket, "output features in bucket.")
        sleep(1)

    stop = time.time()
    print("It took ", stop - start, "seconds to process ", len(df), " videos")

    # Get all features messages
    DownloadMessagesInBucket(my_bucket, key)



if __name__=='__main__':
    main()