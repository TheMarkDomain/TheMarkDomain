#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#ifdef _WIN32
#include <Windows.h>
LPSTR buffer;
DWORD tmp = GetEnvironmentVariable((LPCSTR)("SYSTEMROOT"), buffer, 32768);
std::string buf = (char*)buffer;
const std::string path = buf + "\\System32\\drivers\\etc\\hosts";
#else
const std::string path = "/etc/hosts";
#endif

std::string execute(const char *command) {
    char buffer[32768];
    char result[32768] = {0};
    FILE *ptr;
    strcpy(result, command);
    if ((ptr = popen(result, "r"))) {
        while (fgets(buffer, 32768, ptr)) {
            if (strlen(result) + strlen(buffer) <= 32768) {
                strcat(result, buffer);
            }
        }
        pclose(ptr);
        ptr = NULL;
    }
    return result;
}

struct info {
    int operator<<(std::string x) {
        std::cout << "[INFO] " << x << std::endl;
        return 0;
    }
} info;

struct error {
    int operator<<(std::string x) {
        std::cerr << "[ERROR] " << x << std::endl;
        return 0;
    }
} error;

int main() {
    if (rename(path.c_str(), (path + ".bak").c_str()) == 0) {
        info << "Created hosts file backup.";
    }
    else {
        error << "Failed to create hosts file backup.";
        exit(1);
    }
    std::ofstream file_write(path, std::ios::out | std::ios::trunc);
    if (!file_write.is_open()) {
        error << "Failed to open hosts file.";
        exit(1);
    }
    info << "Writing the hosts file.";
    std::string result = execute("curl -s https://tmd.imjcj.eu.org/hosts");
    result = result.substr(38);
    info << "Complated.";
    file_write << result.data() << std::endl;
    file_write.close();
    std::cout << "You can press Ctrl+Z or Enter to stop the program." << std::endl;
    std::string tmp;
    while (std::getline(std::cin, tmp)) {}
    if (remove(path.c_str()) == 0) {
        info << "Deleted the hosts file.";
    }
    else {
        error << "Failed to delete the hosts file.";
        exit(1);
    }
    if (rename((path + ".bak").c_str(), path.c_str()) == 0) {
        info << "Restored the hosts file.";
    }
    else {
        error << "Failed to restore the hosts file.";
        exit(1);
    }
    return 0;
}