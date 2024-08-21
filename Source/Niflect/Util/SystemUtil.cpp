#include "Niflect/Util/SystemUtil.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

#ifdef _WIN32
#include <direct.h> // for _mkdir
#else
#include <sys/stat.h> // for mkdir
#endif

namespace NiflectUtil
{
    bool create_directory(const Niflect::CString& path) {
        struct stat info;

        // ���·���Ƿ��Ѵ���
        if (stat(path.c_str(), &info) != 0) {
            // ·�������ڣ����Դ���
#ifdef _WIN32
            if (_mkdir(path.c_str()) != 0) {
#else
            if (mkdir(path.c_str(), 0777) != 0) {
#endif
                std::cerr << "Error creating directory: " << path << std::endl;
                return false;
            }
            }
        else if (!(info.st_mode & S_IFDIR)) {
            // ·�����ڵ�����Ŀ¼
            std::cerr << path << " exists but is not a directory." << std::endl;
            return false;
        }
        return true;
    }
    void MakeDirectories(const Niflect::CString& file_path)
    {
        // ��ȡ�ļ�·����Ŀ¼����
        std::string::size_type pos = file_path.find_last_of("/\\");
        if (pos != std::string::npos) {
            auto dir = file_path.substr(0, pos);

            // �ָ�·������㴴��Ŀ¼
            Niflect::TArrayNif<Niflect::CString> path_segments;
            Niflect::CString current_path;
            for (char ch : dir) {
                if (ch == '/' || ch == '\\') {
                    if (!current_path.empty()) {
                        path_segments.push_back(current_path);
                        current_path.clear();
                    }
                }
                else {
                    current_path += ch;
                }
            }
            if (!current_path.empty()) {
                path_segments.push_back(current_path);
            }

            // ����ÿһ��Ŀ¼
            Niflect::CString constructed_path;
            for (const auto& segment : path_segments) {
                constructed_path += segment + "/";
                if (!create_directory(constructed_path)) {
                    std::cerr << "Failed to create path: " << constructed_path << std::endl;
                    return;
                }
            }
        }
    }
}