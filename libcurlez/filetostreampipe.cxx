#include "filetostreampipe.hxx"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

FileToStreamPipe::FileToStreamPipe(std::ostream& stream) : stream(stream)
{
  if (pipe(pipe_fd) == -1)
    throw std::runtime_error(strerror(errno));
  write_handle = fdopen(pipe_fd[1], "w");
  if (write_handle == NULL)
    throw std::runtime_error(strerror(errno));
  fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);
}

FileToStreamPipe::~FileToStreamPipe()
{
  close();
  read_all();
  ::close(pipe_fd[0]);
}

void FileToStreamPipe::read_all()
{
  constexpr size_t buffer_size = 2048;
  char buffer[buffer_size];
  int read_bytes = read(pipe_fd[0], buffer, buffer_size);

  while (read_bytes > 0)
  {
    std::string_view data(buffer, read_bytes);

    stream << data;
    read_bytes = read(pipe_fd[0], buffer, buffer_size);
  }
}

void FileToStreamPipe::flush()
{
  if (write_handle != NULL)
    fflush(write_handle);
}

void FileToStreamPipe::close()
{
  if (write_handle != NULL)
  {
    fflush(write_handle);
    fclose(write_handle);
    ::close(pipe_fd[1]);
    write_handle = NULL;
  }
}
