file_path = "/home/quang/Desktop/ThuMucDemo/duong.txt"

try:
    with open(file_path, 'r') as file:
        data = file.read()
        print(data)
except FileNotFoundError:
    print("Không tìm thấy tệp tin.")
except IOError:
    print("Lỗi khi đọc tệp tin.")
