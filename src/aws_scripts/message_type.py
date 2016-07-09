
def create_message(s3_path, classification): 
    """
    This function creates an s3 message that is used to add
    new messages on the SQS queue
    """
    mesg = {
        'MessageBody': s3_path,
        'MessageAttributes': {
            'Classification': {
                'StringValue': str(classification),
                'DataType': 'Number'
            }
        }
    }
    return mesg

def create_messages(list_of_files, classification):
    """
    Create multiple messages off of the key value pair:
    e.g. key_val_pairs =[{'path': 'test1', 'classification': 1},{'path': 'test2', 'classification':2}]
    :param key_val_pairs:
    :return:
    """
    mesgs = []
    for idx, file in enumerate(list_of_files):
         mesg = {
            'Id': str(idx),
            'MessageBody': file,
            'MessageAttributes': {
                'Classification': {
                    'StringValue': str(classification),
                    'DataType': 'Number'
                }
            }
        }
         mesgs.append(mesg)

    return mesgs

def create_messages_from_df(df):
    """
    Create a message to send to SQS from a pandas data frame
    :param df: Dataframe consisting of ['path', 'classification', 's3_output']
    :return: Returns a message that can be sent with boto3 to sqs
    """
    max_size = 10
    messages = []
    batch_messages = []
    for index, series in df.iterrows():
        mod_index = index%max_size #Max of 10 messages can be sent in a single batch process.
        message = {
            'Id': str(mod_index),
            'MessageBody': series['path'],
            'MessageAttributes': {
                'Classification': {
                    'StringValue': str(series['classification']),
                    'DataType': 'Number'
                },
                'SQSQueue': {
                    'StringValue': str(series['sqs_queue']),
                    'DataType': 'String'
                }
            }
        }
        messages.append(message)
        if len(messages) == max_size :
            print("len messages = max size")
            batch_messages.append(messages)
            messages=[]

    if len(messages) != max_size:
        batch_messages.append(messages)


    return batch_messages
