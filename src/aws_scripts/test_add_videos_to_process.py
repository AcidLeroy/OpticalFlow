from add_videos_to_process import PopulateMessageList
from mock import Mock


def test_MessageAlreadyReceived():
    new_message = 'file1,123124,123123'
    expected_files = ['file1', 'file2']
    message_list = []
    expected_files, message_list, added_message = PopulateMessageList(expected_files, message_list, new_message)
    assert ['file2'] == expected_files
    assert ['file1,123124,123123'] == message_list
    assert True == added_message
    expected_files, message_list, added_message = PopulateMessageList(expected_files, message_list, new_message)
    assert ['file2'] == expected_files
    assert ['file1,123124,123123'] == message_list
    assert False == added_message
    new_message = 'file2,123124,123123'
    expected_files, message_list, added_message = PopulateMessageList(expected_files, message_list, new_message)
    assert [] == expected_files
    assert ['file1,123124,123123', 'file2,123124,123123'] == message_list
    assert True == added_message











