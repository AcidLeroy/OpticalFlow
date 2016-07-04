#!/bin/env python

from __future__ import print_function

import argparse
import os
import pandas as pd
import message_type as mt
import boto3

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




if __name__=='__main__':
    main()