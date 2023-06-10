import os
import subprocess
import tkinter as tk
from tkinter import messagebox

# setup driver
DEVICE_PATH_0 = "/dev/test"

# Thay đổi đường dẫn thư mục tại đây
directory_path = "/home/quang/Desktop/ThuMucDemo/"


def read_from_driver(DEVICE_PATH):
    with open(DEVICE_PATH, "r") as file:
        data = file.read()
        return data


def encode_file():
    selected_file = file_listbox.get(tk.ACTIVE)
    data = read_from_driver(DEVICE_PATH_0)
    if selected_file:
        subprocess.run(["zenity", "--info", f"--text=Mã hóa tệp tin: {data}"])
        os.remove(directory_path + selected_file)
        refresh_file_list()  # Làm mới danh sách tệp tin sau khi xóa
        # Thực hiện mã hóa tệp tin ở đây


def decode_file():
    selected_file = file_listbox.get(tk.ACTIVE)
    if selected_file:
        # subprocess.run(["zenity", "--info", f"--text=Giải mã tệp tin: {selected_file}"])
        file_path = directory_path + selected_file
        #mo file bằng sublime
        subprocess.run(["subl", file_path])
        # Thực hiện giải mã tệp tin ở đây


def refresh_file_list():
    file_listbox.delete(0, tk.END)  # Xóa tất cả các tệp tin trong danh sách
    files = os.listdir(directory_path)
    for file in files:
        file_listbox.insert(tk.END, file)


def show_context_menu(event):
    popup_menu.post(event.x_root, event.y_root)


# Tạo cửa sổ tkinter
window = tk.Tk()
window.title("File Manager")
window.geometry("700x400")


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
#popup_menu.add_command(label="Mã hóa bang AES", command=encode_file)
#popup_menu.add_command(label="Mã hóa DES", command=encode_file)

# Thiết lập sự kiện chuột phải để hiển thị menu
file_listbox.bind("<Button-3>", show_context_menu)

# Lấy danh sách các tệp tin trong thư mục và hiển thị lên listbox
files = os.listdir(directory_path)
for file in files:
    file_listbox.insert(tk.END, file)

window.mainloop()
