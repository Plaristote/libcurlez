#pragma once
#include <cstdlib>
#include <cstdio>
#include <ostream>

class FileToStreamPipe
{
  int   pipe_fd[2];
  FILE* write_handle = NULL;
  std::ostream& stream;
public:
  FileToStreamPipe(std::ostream& stream);
  ~FileToStreamPipe();

  FILE* handle() const { return write_handle; }

  void read_all();
  void flush();
  void close();
};
