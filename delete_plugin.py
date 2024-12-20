import sys
import os
import shutil

plugin_name = ""


def erase_line(file_path, key_word):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    with open(file_path, 'w', encoding='utf-8') as file:
        for line in lines:
            if line.find(key_word) != -1:
                pass
            else:
                file.write(line)


def erase_major_cmake_file():
    erase_line("CMakeLists.txt", plugin_name)


def erase_plugin_instance_header_file():
    erase_line("Host/PluginInstanceIncludedHeader.inl", plugin_name)


def erase_plugin_instance_factory_file():
    erase_line("Host/PluginInstanceFactoryCreation.inl", plugin_name)


def erase_cmake_link_library_file():
    erase_line("Host/CMakeLinkLibraries.cmake", plugin_name)


def delete_folder():
    files = os.listdir("./")
    for file in files:
        if os.path.isdir(os.path.join("./", file)):
            if file.find(".git") != -1:
                continue
            if file.find(plugin_name) != -1:
                shutil.rmtree(file)
                return


def print_usage():
    print(
        "python3 delete_plugin [plugin_name],such as:python3 delete_plugin Delay")
    exit(1)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print_usage()

    plugin_name = sys.argv[1]

    erase_major_cmake_file()
    erase_plugin_instance_header_file()
    erase_plugin_instance_factory_file()
    erase_cmake_link_library_file()

    delete_folder()
