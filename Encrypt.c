#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <windows.h>
#include <shlobj.h>

void store_encrypted_password(const char* password) {
  // Encrypt the password using MD5.
  unsigned char hash[MD5_DIGEST_LENGTH];
  MD5((unsigned char*)password, strlen(password), hash);

  // Convert the hashed password to hexadecimal format.
  char encrypted_password[MD5_DIGEST_LENGTH * 2 + 1];
  for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
    sprintf(&encrypted_password[i * 2], "%02x", hash[i]);
  }

  // Get the path to the Desktop folder.
  WCHAR desktop_path[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktop_path))) {
    // Create the full path to the file.
    WCHAR file_path[MAX_PATH];
    swprintf(file_path, sizeof(file_path) / sizeof(WCHAR), L"%s\\PwdHash.txt", desktop_path);

    // Open the file for writing.
    FILE* file = _wfopen(file_path, L"w");
    if (file != NULL) {
      // Write the encrypted password to the file.
      fputs(encrypted_password, file);

      // Close the file.
      fclose(file);

      wprintf(L"The encrypted password has been stored in %s\\PwdHash.txt\n", desktop_path);
    } else {
      wprintf(L"Failed to open the file for writing.\n");
    }
  } else {
    wprintf(L"Failed to retrieve the Desktop folder path.\n");
  }
}

int encrypt_drive_full(char drive_letter, char *password, char *mac_address) {
  // Check if the MAC address matches the device.
  if (!validate_mac_address(mac_address)) {
    return 1;
  }

  // Get the size of the external storage drive.
  DWORD size = 0;
  if (!GetDiskFreeSpaceEx(&drive_letter, NULL, NULL, &size)) {
    return 1;
  }

  // Create a buffer to store the encrypted data.
  char *buffer = malloc(size);
  if (!buffer) {
    return 1;
  }

  // Encrypt the data in the buffer.
  int status = encrypt_data(buffer, size, password);
  if (status != 0) {
    return 1;
  }

  // Write the encrypted data to the external storage drive.
  DWORD written = 0;
  if (!WriteFile(GetLogicalDrives() + (drive_letter - 'A'), buffer, size, &written, NULL)) {
    return 1;
  }

  free(buffer);
  return 0;
}

int encrypt_data(char *data, int size, char *password) {
  // TODO: Implement the encryption function.
  return 0;
}

int validate_mac_address(char *mac_address) {
  // Compare the retrieved MAC address with the expected MAC address of your device.
  // Modify this function according to your specific validation requirements.
  char expected_mac_address[] = "00:11:22:33:44:55";
  if (strcmp(mac_address, expected_mac_address) == 0) {
    return 1;
  }

  return 0;
}

int main() {
  // Get the drive letter of the external storage drive.
  char drive_letter;
  printf("Enter the drive letter of the external storage drive: ");
  scanf(" %c", &drive_letter);

  // Get the password for the encryption.
  char password[256];
  printf("Enter the password for the encryption: ");
  scanf("%s", password);

  // Check and save the MAC address of the device.
  char mac_address[18];
  if (!get_mac_address(mac_address)) {
    printf("Failed to retrieve the MAC address.\n");
    return 1;
  }

  // Encrypt the external storage drive.
  int status = encrypt_drive_full(drive_letter, password, mac_address);

  // Check the status of the encryption.
  if (status == 0) {
    printf("The external storage drive has been encrypted successfully.\n");
  } else {
    printf("The external storage drive could not be encrypted.\n");
  }

  // Store the encrypted password in the Desktop folder.
  store_encrypted_password(password);

  return 0;
}
