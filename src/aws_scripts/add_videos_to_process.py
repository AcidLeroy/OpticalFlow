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

def InitializeOutputQueue(queue_name):
    sqs = boto3.resource('sqs')
    output_queue = sqs.create_queue(QueueName=queue_name)
    try:
        output_queue.purge()
    except:
        print("Already purged...")
    return output_queue

def ReceiveNMessagesFromOutputQueue(output_queue, number_messages):
    """
    Function that polls the queue for N number of messages. This loop does not
    exit until all messages from the output queue have been received
    :param output_queue: The queue to poll for CDFs
    :param number_messages: The number of messages to expect to receive before exiting
    :return: list of all the cdfs calculated
    """
    messages_received = 0
    message_list = []
    while messages_received != number_messages:
        for message in output_queue.receive_messages():
            print("Received: ", message.body)
            messages_received = messages_received + 1
            message_list.append(message.body)
            message.delete()
    return message_list




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
    print("Using queue = ", queue_name, " for putting video paths")

    # Set up the queue for receiving the output of the compute nodes
    output_queue = InitializeOutputQueue("feature_queue")

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



    # Wait for input queue to become empty
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

    # Retrieve messages from the sqs queue
    total_features = len(df)

    # Poll until all the messages have been received.
    message_list = ReceiveNMessagesFromOutputQueue(output_queue, total_features)

    stop = time.time()
    print("It took ", stop - start, "seconds to process ", len(df), " videos")


if __name__=='__main__':
    main()