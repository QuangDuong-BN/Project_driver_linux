import os

# Đường dẫn đến thiết bị
DEVICE_PATH = "/dev/encryption_device"

# Kích thước key
KEY_SIZE = 16

# Dữ liệu gốc
original_data = b"Hellodwd"

# Mở thiết bị
device_file = os.open(DEVICE_PATH, os.O_RDWR)

# Gửi key tới thiết bị
key = os.urandom(KEY_SIZE)
os.write(device_file, key)

# Gửi dữ liệu tới thiết bị để mã hóa
os.write(device_file, original_data)

# Đọc dữ liệu đã mã hóa từ thiết bị
encrypted_data = os.read(device_file, len(original_data))

# Giải mã dữ liệu
os.lseek(device_file, 0, os.SEEK_SET)
os.write(device_file, key)
decrypted_data = os.read(device_file, len(original_data))

# Đóng thiết bị
os.close(device_file)

print("Original data:", original_data)
print("Encrypted data:", encrypted_data)
print("Decrypted data:", decrypted_data)
