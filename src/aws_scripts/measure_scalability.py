from __future__ import print_function
import add_videos_to_process as proc
import boto3
import botocore
from time import sleep
import time
import os


group_name = 'FeatureExtractor'

def create_group():
    client = boto3.client('autoscaling')
    response = client.create_auto_scaling_group(
        AutoScalingGroupName=group_name,
        InstanceId='i-16f549b9',
        MinSize=1,
        MaxSize=20,
        DesiredCapacity=1,
        DefaultCooldown=1
    )

def get_running_tasks():
    client = boto3.client('ecs')
    response = client.describe_clusters(
        clusters=[
            'default',
        ]
    )
    return response['clusters'][0]['runningTasksCount']

def set_number_tasks(size):
    client = boto3.client('ecs')
    response = client.update_service(
        cluster='default',
        service='matlab_extract_features',
        desiredCount=size,

    )


def set_capacity(size):
    client = boto3.client('autoscaling')
    response = client.set_desired_capacity(
        AutoScalingGroupName=group_name,
        DesiredCapacity=size,
        HonorCooldown=False
    )

def enable_metrics():
    client = boto3.client('autoscaling')
    response = client.enable_metrics_collection(
        AutoScalingGroupName=group_name,
        Metrics=[
            'GroupTotalInstances',
        ],
        Granularity='1Minute'
    )


def get_running_instances():
    client = boto3.client('autoscaling')
    response = client.describe_auto_scaling_groups(
        AutoScalingGroupNames=[
            group_name,
        ]
    )
    counter = 0
    instances = response['AutoScalingGroups'][0]['Instances']
    for inst in instances:
        if inst['LifecycleState']  == 'InService':
            counter = counter + 1

    return counter

def set_and_wait(desired_count):

    set_number_tasks(desired_count)
    print("The desired count is", desired_count)
    while not(get_running_tasks() == desired_count):
        print("Current number of instances = ", get_running_instances())
        print("Current number of tasks = " , get_running_tasks())
        sleep(30)
    print("Reached desired count!")
    print("Desired tasks = ", get_running_tasks())
    print("*"*100)
    print()




def main():
    file_name = 'scalability_results.txt'
    try:
        os.remove(file_name)
    except:
        print("No file to remove...")
    set_capacity(20)
    try:
        create_group()
    except botocore.exceptions.ClientError as e:
        print('Already created...', e)
    enable_metrics()
    desired = [x for x in range(1, 21) if 20 % x == 0]
    with open(file_name, 'a') as f:
        f.write('Number of Instances,Time to Process (s)\n')
        for i in desired:
            set_and_wait(i)
            start = time.time()
            proc.add_videos('twenty_videos.txt', 'twenty_videos_output.csv', 'video_list')
            end = time.time()
            f.write('{},{}\n'.format(i, end-start))


if __name__ == '__main__':
    main()