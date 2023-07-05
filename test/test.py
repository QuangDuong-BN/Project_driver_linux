import os
import subprocess
import tkinter as tk
from tkinter import messagebox
import hashlib
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding


def aes_encrypt(plaintext, key):
    iv = b'0123456789abcdef'
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    padder = padding.PKCS7(algorithms.AES.block_size).padder()
    padded_plaintext = padder.update(plaintext) + padder.finalize()
    ciphertext = encryptor.update(padded_plaintext) + encryptor.finalize()
    return ciphertext


def aes_decrypt(ciphertext, key):
    iv = b'0123456789abcdef'
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    decryptor = cipher.decryptor()
    unpadder = padding.PKCS7(algorithms.AES.block_size).unpadder()
    padded_plaintext = decryptor.update(ciphertext) + decryptor.finalize()
    plaintext = unpadder.update(padded_plaintext) + unpadder.finalize()
    return plaintext


def generate_key_from_text(text):
    key = hashlib.sha256(text.encode('utf-8')).digest()
    return key


# setup driver
# DEVICE_PATH_0 = "/dev/test"

# Thay đổi đường dẫn thư mục tại đây
directory_path = "/home/quang/Desktop/ThuMucDemo/"

# Biến toàn cục để lưu mã key
key = ""
dataE = ""
data=""

def read_from_driver(DEVICE_PATH):
    with open(DEVICE_PATH, "r") as file:
        data = file.read()
        return data


def encode_file():
    selected_file = file_listbox.get(tk.ACTIVE)
    # data = read_from_driver(DEVICE_PATH_0)
    file_path = directory_path + selected_file
    if selected_file:
        try:
            with open(file_path, 'rb') as file:
                data = file.read()
                data = aes_encrypt(data, generate_key_from_text(key))
            with open(file_path, 'wb') as file:
                file.write(data)
                messagebox.showinfo("Thông báo", "Hoàn thành mã hóa")
        except FileNotFoundError:
            print("Không tìm thấy tệp tin.")
        except IOError:
            print("Lỗi khi đọc tệp tin.")


def decode_file():
    selected_file = file_listbox.get(tk.ACTIVE)
    # data = read_from_driver(DEVICE_PATH_0)
    file_path = directory_path + selected_file
    if selected_file:
        try:
            with open(file_path, 'rb') as file:
                data = file.read()
                data = aes_decrypt(data, generate_key_from_text(key))
            with open(file_path, 'wb') as file:
                file.write(data)
                messagebox.showinfo("Thông báo", "Hoàn thành Giải mã")
        except Exception:
            messagebox.showinfo("Thông báo", "Giải mã thất bại")



def open_file():
    selected_file = file_listbox.get(tk.ACTIVE)
    if selected_file:
        file_path = directory_path + selected_file
        # mo file bằng sublime
        subprocess.run(["subl", file_path])
        # Thực hiện giải mã tệp tin ở đây


def refresh_file_list():
    file_listbox.delete(0, tk.END)  # Xóa tất cả các tệp tin trong danh sách
    files = os.listdir(directory_path)
    for file in files:
        file_listbox.insert(tk.END, file)


def show_context_menu(event):
    popup_menu.post(event.x_root, event.y_root)


def save_key():
    global key
    key = key_entry.get()  # Lưu giá trị nhập vào biến key
    messagebox.showinfo("Thông báo", "Mã key đã được lưu thành công!")


# Tạo cửa sổ tkinter
window = tk.Tk()
window.title("File Manager")
window.geometry("700x400")

# Tạo trường nhập liệu cho mã key
key_entry = tk.Entry(window, width=50)
key_entry.pack(pady=10)

# Tạo nút lưu mã key
save_button = tk.Button(window, text="Lưu Mã", command=save_key)
save_button.pack()

# Tạo danh sách listbox để hiển thị các tệp tin
file_listbox = tk.Listbox(window, width=100)
file_listbox.pack(fill=tk.BOTH, expand=True)

# Tạo thanh cuộn
scrollbar = tk.Scrollbar(file_listbox)
scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

# Kết nối thanh cuộn với listbox
file_listbox.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=file_listbox.yview)

# Tạo menu chuột phải
popup_menu = tk.Menu(window, tearoff=0)
popup_menu.add_command(label="Mã hóa", command=encode_file)
popup_menu.add_command(label="Giải mã", command=decode_file)
popup_menu.add_command(label="mở bằng sublime Text", command=open_file)
# Thiết lập sự kiện chuột phải để hiển thị menu
file_listbox.bind("<Button-3>", show_context_menu)

# Lấy danh sách các tệp tin trong thư mục và hiển thị lên listbox
files = os.listdir(directory_path)
for file in files:
    file_listbox.insert(tk.END, file)

window.mainloop()
