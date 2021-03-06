#!/usr/bin/env python

from __future__ import print_function

import argparse
import os
import pandas as pd
import message_type as mt
import boto3
from time import sleep
import time
import sys

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

def PopulateMessageList(expected_files, message_list, current_message):
    # Just need to assume that first value is the filename
    message_filename = current_message.split(',')[0]
    for idx, filename in enumerate(expected_files):
        if filename == message_filename:
            message_list.append(current_message)
            del expected_files[idx]
            return (expected_files, message_list, True)
    return (expected_files, message_list, False)

def ReceiveNMessagesFromOutputQueue(output_queue, expected_files):
    """
    Function that polls the queue for messages.
    expected_files: This is a list of the files we expect to get back from the queue. We only want to get
    messages we are actually expecting.
    """
    try:
        message_list = []
        number_messages = len(expected_files)
        counter = 1
        while len(message_list) != number_messages:
            for message in output_queue.receive_messages(MaxNumberOfMessages=10):
                expected_files, message_list, added_message = PopulateMessageList(expected_files, message_list, message.body)
                # Only delete the message if we added it our list, otherwise leave it.
                if added_message:
                    message.delete()
            if 10 % counter == 0:
                OKGREEN = '\033[92m'
                ENDC = '\033[0m'
                print("Waiting for the following files to be processed:", end="")
                print(OKGREEN, end="")
                print('\n\t', end="")
                print(('\n\t').join(expected_files))
                print(ENDC, end="")
                print()
                sys.stdout.flush()
            sleep(1)
    except KeyboardInterrupt as e:
        print("Stopping loop even though all messages were not received...")

    return message_list



def main():
    parser = argparse.ArgumentParser(description="A utility to add a list of video files that exist on Amazon S3 from which to extract features.")
    parser.add_argument("input_file", type=str, help="Path to a file that contains a list"
                                                     " of S3 files to process. File must be a comma separated listed of path,classification.")
    parser.add_argument('--queue_name', type=str, default='default', help='Name of the queue to use for adding the messages. '
                                                                          'If the queue does not already exist, one is created.'
                                                                          ' The default queue is named \'default\'.')
    parser.add_argument('--output_file', type=str, default='output.csv', help='Name of the file to store the results')

    args = parser.parse_args()


    input_file = os.path.abspath(args.input_file)
    queue_name = args.queue_name
    output_file = os.path.abspath(args.output_file)

    add_videos(input_file, output_file, queue_name)


def add_videos(input_file, output_file, queue_name):
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
    zero_counter = 1
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
    filenames_to_receive = list(df['path'])
    # Poll until all the messages have been received.
    print("Wating for all messages to be processed...")
    message_list = ReceiveNMessagesFromOutputQueue(output_queue, filenames_to_receive)
    print("Done!")
    stop = time.time()
    print("It took ", stop - start, "seconds to process ", len(message_list), " videos")
    # save messages to a file
    header = ['Filename,CenX_CDF,CenY_CDF,Orient_CDF,Histo_CDF,Motion_mag_CDF,Motion_orient_CDF,Classification\n']
    message_list = header + message_list
    message_list = ('').join(message_list)
    with open(output_file, 'wb') as f:
        f.write(message_list)


if __name__=='__main__':
    main()
