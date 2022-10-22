#pragma once

#include <cassert>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class FileHandle {
    FILE *m_file_ptr;
    std::string m_pathname;

  public:
    FileHandle() {
        m_file_ptr = nullptr;
    }

    FileHandle(std::string pathname) {
        // should probably create some kind of mutex on the file itself
        if ((m_file_ptr = fopen(pathname.data(), "a+")) == nullptr) {
            int errsv = errno;
            std::cerr << "FileHandle Constructor: Error opening file. " << strerror(errsv) << std::endl;
            exit(1);
        }
        reset_to_beginning();

        m_pathname = std::move(pathname);
    }

    // when the handle goes out of scope, we close the file as well
    ~FileHandle() {
        if (m_file_ptr != nullptr) {
            fclose(m_file_ptr);
        }
    }

    // for now we delete this but eventually we might need a smart way around this for
    // when multiview is a thing
    FileHandle(FileHandle const &) = delete;
    FileHandle &operator=(FileHandle const &) = delete;

    friend void swap(FileHandle &a, FileHandle &b) {
        std::swap(a.m_file_ptr, b.m_file_ptr);
        std::swap(a.m_pathname, b.m_pathname);
    }

    // do we want to make FileHandles movable? perhaps
    FileHandle(FileHandle &&other) : m_file_ptr(other.m_file_ptr), m_pathname(std::move(other.m_pathname)) {
        other.m_file_ptr = nullptr;
        other.m_pathname.clear();
    }

    FileHandle &operator=(FileHandle &&other) {
        // I don't think I need the temp here actually
        FileHandle temp{std::move(other)};
        using std::swap;
        swap(*this, temp);
        return *this;
    }

    void reset_to_beginning() {
        assert(m_file_ptr != nullptr);
        // reset the file position to the beginning
        // if (fseek(m_file_ptr, 0, SEEK_SET) == -1) {
        //     int errsv = errno;
        //     std::cerr << "FileHandle save(): Error seeking to beginning of file. " << strerror(errsv)
        //               << std::endl;
        //     exit(1);
        // }
        rewind(m_file_ptr);
    }

    void clear_file() {
        truncate(m_pathname.data(), 0);
    }

    void save(std::string to_write) {
        if (m_file_ptr == nullptr) {
            std::runtime_error("FileHandle save(): Can't save to an unspecified file!");
        }

        clear_file();
        reset_to_beginning();

        size_t written_count = fwrite(to_write.data(), sizeof(char), to_write.size(), m_file_ptr);
        if (written_count < to_write.size()) {
            std::cerr << "FileHandle save(): Error writing to file. ";
        }
    }

    void open(std::string pathname) {
        // not sure if this is the right way of doing it... hmm.
        *this = FileHandle(std::move(pathname));
    }

    bool is_handle_to_file() const {
        return m_file_ptr != nullptr;
    }

    std::string read() {
        if (m_file_ptr == nullptr) {
            std::runtime_error("FileHandle read(): Can't read an unspecified file!");
        }
        reset_to_beginning();

        struct stat statbuf;
        if (stat(m_pathname.data(), &statbuf) == -1) {
            int errsv = errno;
            std::cerr << "FileHandle read(): Error trying to stat file. " << strerror(errsv) << std::endl;
        }
        std::string to_return;
        to_return.resize(statbuf.st_size);

        ssize_t read_count = fread(to_return.data(), sizeof(char), statbuf.st_size, m_file_ptr);
        if (read_count < statbuf.st_size) {
            std::cerr << "FileHandle read(): Error trying to read from file. " << std::endl;
        }
        return to_return;
    }

    std::string pathname() const {
        return m_pathname;
    }
};