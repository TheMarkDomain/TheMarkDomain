#include <iostream>
#include <string>
#include <fstream>
using namespace std;
#ifdef _WIN32
const string path = to_string(getenv("SystemRoot")) + "\\System32\\drivers\\etc\\hosts";
#else
const string path = "/etc/hosts";
#endif

string execute(const char *command) {
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
    info operator<<(string x) {
        cout << "[INFO] " << x << endl;
        return *this;
    }
} info;

struct error {
    error operator<<(string x) {
        cerr << "[ERROR] " << x << endl;
        return *this;
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
    ifstream file_write(path, ios::out | ios::trunc);
    if (!file_write.is_open()) {
        error << "Failed to open hosts file.";
        exit(1);
    }
    info << "Writing the hosts file.";
    string result = execute("curl -s https://tmd.imjcj.eu.org/hosts");
    result = result.substr(38);
    info << "Complated.";
    file_write << result << endl;
    file_write.close();
    cout << "You can press Ctrl+Z or Enter to stop the program." << endl;
    string tmp;
    while (getline(cin, tmp)) {}
    if (remove(path) == 0) {
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