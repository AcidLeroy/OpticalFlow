from __future__ import print_function
import boto3
import subprocess
import os
import pandas as pd
import time

def DownloadFile(local_file, bucket, bucket_path):
    print("Downloading file...", end="")
    s3 = boto3.resource('s3')
    s3.Bucket(bucket).download_file(bucket_path, local_file)
    print("Done!")

def UploadFile(local_file, bucket, bucket_path):
    print("Uploading file...", end="")
    s3 = boto3.resource('s3')
    s3.Bucket(bucket).upload_file(local_file, bucket_path)
    print("Done!")

def CreateFile(file_size):
    file_size = file_size *1e6
    file_name = 'file.bin'
    subprocess.check_output(['dd', 'if=/dev/zero', 'of='+file_name, 'bs='+str(int(file_size)), 'count=1'])
    return os.path.abspath(file_name)


def main():
    file_sizes_mb = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
    #file_sizes_mb = [1, 2, 4, 8]
    num_iterations=10
    bucket = 'aolme'
    bucket_path = 'dummy/file.bin'

    times ={}
    for sz in file_sizes_mb:
        local_file = CreateFile(sz)
        upload_times =[]
        download_times = []
        print("Testing size: " + str(sz))
        for iter in range(1, num_iterations+1):
            start = time.time()
            UploadFile(local_file, bucket, bucket_path)
            elapsed_time = time.time() - start
            upload_times.append(elapsed_time)

            start = time.time()
            DownloadFile(local_file, bucket, bucket_path)
            elapsed_time = time.time() - start
            download_times.append(elapsed_time)

        times['Download ' + str(sz) + 'MB (s)']= download_times
        times['Upload ' + str(sz) + 'MB (s)'] = upload_times
    df = pd.DataFrame(times)
    print(df.head())
    df.to_csv("s3_timing_results.txt")

if __name__=='__main__':
    main()
