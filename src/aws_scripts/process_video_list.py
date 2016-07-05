
#!/bin/env python

from __future__ import print_function
import argparse
import boto3
import uuid

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

    args = parser.parse_args()
    queue_name = args.queue_name
    print("Using queue name: ", queue_name)



    # Get the SQS messages
    sqs = boto3.resource('sqs')
    queue = sqs.get_queue_by_name(QueueName=queue_name)

    # Top level bucket directory to use for s3
    default_output_dir="featureoutput"
    s3 = boto3.resource('s3')
    bucket = s3.Bucket(default_output_dir)
    s3_client = boto3.client('s3')


    while(True):
        for message in queue.receive_messages(MessageAttributeNames=['Classification','S3Output']):
            classification = message.message_attributes.get('Classification').get('StringValue')
            s3_output = message.message_attributes.get('S3Output').get('StringValue')
            output_string = "Path = "+ message.body+ " with classification = "+ classification+ " Goes to path: "+ \
                            s3_output + '\n'
            print(output_string)

            #Write the results to s3
            filename = str(uuid.uuid1()) + '.csv'
            with open(filename, 'w') as f:
                f.write(output_string)

            file_path = s3_output + '/' + filename
            s3_client.upload_file(filename, default_output_dir, file_path)

            message.delete()


if __name__=="__main__":
    main()

