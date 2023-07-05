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

# Chuỗi văn bản
text = "mysecretkey12345"

# Văn bản cần mã hóa
plaintext = b"Hello, AES encryptionfffffffff!"

# Tạo khóa từ chuỗi văn bản
key = generate_key_from_text(text)

# Mã hóa
encrypted = aes_encrypt(plaintext, key)
print('Encrypted:', encrypted)

# Giải mã
decrypted = aes_decrypt(encrypted, key)
print('Decrypted:', decrypted)
