/* ************************************************************************
 * Copyright 2019-2021 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "hipblas_parse_data.hpp"
#include "utility.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/types.h>

#ifdef __cpp_lib_filesystem
#include <filesystem>
#else
#include <experimental/filesystem>

namespace std
{
    namespace filesystem = experimental::filesystem;
}
#endif

// Parse YAML data
static std::string hipblas_parse_yaml(const std::string& yaml)
{
    // Generate "/tmp/hipblas-XXXXXX" like file name
    const std::string alphanum     = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv";
    int               stringlength = alphanum.length() - 1;
    std::string       uniquestr    = "hipblas-";

    for(auto n : {0, 1, 2, 3, 4, 5})
        uniquestr += alphanum.at(rand() % stringlength);

    std::filesystem::path tmpname = std::filesystem::temp_directory_path() / uniquestr;

    auto exepath = hipblas_exepath();
    // TODO:
    //auto cmd = exepath + "hipblas_gentest.py --template " + exepath + "hipblas_template.yaml -o "
    //           + tmpname.string() + " " + yaml;
    //std::cerr << cmd << std::endl;
    //int status = std::system(cmd.c_str());

    return tmpname.string(); // results to be read and removed later
}

// Parse --data and --yaml command-line arguments
bool hipblas_parse_data(int& argc, char** argv, const std::string& default_file)
{
    std::string filename;
    char**      argv_p = argv + 1;
    bool        help = false, yaml = false;

    // Scan, process and remove any --yaml or --data options
    for(int i = 1; argv[i]; ++i)
    {
        if(!strcmp(argv[i], "--data") || !strcmp(argv[i], "--yaml"))
        {
            if(!strcmp(argv[i], "--yaml"))
            {
                yaml = true;
            }

            if(filename != "")
            {
                std::cerr << "Only one of the --yaml and --data options may be specified"
                          << std::endl;
                exit(EXIT_FAILURE);
            }

            if(!argv[i + 1] || !argv[i + 1][0])
            {
                std::cerr << "The " << argv[i] << " option requires an argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            filename = argv[++i];
        }
        else
        {
            *argv_p++ = argv[i];
            if(!help && (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")))
            {
                help = true;
                std::cout << "\n"
                          << argv[0] << " [ --data <path> | --yaml <path> ] <options> ...\n"
                          << std::endl;
            }
        }
    }

    // argc and argv contain remaining options and non-option arguments
    *argv_p = nullptr;
    argc    = argv_p - argv;

    if(filename == "-")
        filename = "/dev/stdin";
    else if(filename == "")
        filename = default_file;

    if(yaml)
        filename = hipblas_parse_yaml(filename);

    if(filename != "")
    {
        // TODO
        //RocBLAS_TestData::set_filename(filename, yaml);
        return true;
    }

    return false;
}
