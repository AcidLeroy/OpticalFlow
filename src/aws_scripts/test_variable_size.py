import re
import add_videos_to_process as proc
import time

def replace_size_in_file(file_name, size):
    with open(file_name, 'r') as f:
        data = f.read()

    new_data = re.sub(r'mb\d+\.mov', 'mb'+size+'.mov', data)
    with open(file_name, 'w') as f:
        f.write(new_data)


def main():
    file_name = 'variable_size_files.txt'
    x = [1, 2, 4, 8, 16, 32, 64]
    x_str = [str(i) for i in x]
    with open('variable_size_timing_results.txt', 'w') as f:
        f.write('Size MB,time (s)\n')
        for size in x_str:
            replace_size_in_file(file_name, size)
            start = time.time()
            proc.add_videos(file_name, 'variable_size_output_' + size + '.csv', 'video_list')
            elapsed_time = time.time() - start
            f.write('{},{}\n'.format(size, elapsed_time))

if __name__ == '__main__':
    main()



