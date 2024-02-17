#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#ifdef _WIN32
#include <Windows.h>
const std::string path = "C:\\Windows\\System32\\drivers\\etc\\hosts";
#else
const std::string path = "/etc/hosts";
#endif
bool flag = false;
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_2->setOpenExternalLinks(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (!flag) {
        if (rename(path.c_str(), (path + ".bak").c_str()) != 0) {
            ui -> label_2 -> setText("无法创建备份，请使用管理员或root身份运行");
            return;
        }
        std::ofstream file_write(path, std::ios::out | std::ios::trunc);
        if (!file_write.is_open()) {
            ui -> label_2 -> setText("无法写入文件，请使用管理员或root身份运行");
            return;
        }
        std::string result = execute("curl -s https://tmd.imjcj.eu.org/hosts");
        result = result.substr(38);
        file_write << result.data() << std::endl;
        file_write.close();
        ui -> label_2 -> setText("已启用 <a href='http://tmd.mark/'>测试一下</a>");
        flag = true;
    }
    else {
        if (remove(path.c_str()) != 0) {
            ui -> label_2 -> setText("无法删除现文件，请使用管理员或root身份运行");
            return;
        }
        if ( rename((path + ".bak").c_str(), path.c_str()) != 0) {
            ui -> label_2 -> setText("无法恢复备份，请使用管理员或root身份运行");
            return;
        }
        ui -> label_2 -> setText("已停止");
        flag = false;
    }
}
