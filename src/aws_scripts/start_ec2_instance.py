import boto3

instance_id = 'i-16f549b9'

def stop():
    ec2 = boto3.resource('ec2')
    instance = ec2.Instance(instance_id)
    instance.stop()

def start():
    ec2 = boto3.resource('ec2')
    instance = ec2.Instance(instance_id)
    instance.start()

def main():
    start()




if __name__ == '__main__':
    main()