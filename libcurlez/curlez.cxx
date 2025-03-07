#include <libcurlez/curlez.hxx>
#include <libcurlez/filetostreampipe.hxx>
#include <sstream>
#include <cstring>

using namespace std;

Curl::Curl()
{
  handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &Curl::write_data);
  curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, &Curl::header_callback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, reinterpret_cast<void*>(this));
  curl_easy_setopt(handle, CURLOPT_HEADERDATA, reinterpret_cast<void*>(this));
}

Curl::~Curl()
{
  if (headers)
    curl_slist_free_all(headers);
  curl_easy_cleanup(handle);
}

Curl::Curl(Curl&& b)
{
  handle = b.handle;
  headers = b.headers;
  b.headers = nullptr;
  b.handle = curl_easy_init();
}

Curl& Curl::method(const std::string& method_)
{
  curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, method_.c_str());
  return *this;
}

Curl& Curl::url(const std::string& url_)
{
  curl_easy_setopt(handle, CURLOPT_URL, url_.c_str());
  return *this;
}

Curl& Curl::body(const std::string_view data)
{
  return body(std::string(data));
}

Curl& Curl::body(const std::string& data)
{
  header("Content-Length", data.length());
  curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data.length());
  return *this;
}

Curl& Curl::header(const std::string_view name, std::string_view value)
{
  stringstream entry;

  entry << name << ": " << value;
  headers = curl_slist_append(headers, entry.str().c_str());
  return *this;
}

Curl& Curl::header(const std::string_view name, const std::string& value)
{
  return header(name, string_view(value.c_str(), value.length()));
}

Curl& Curl::header(const std::string_view name, const char* value)
{
  return header(name, string_view(value, strlen(value)));
}

Curl& Curl::follow_redirects()
{
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  return *this;
}

Curl& Curl::ignore_ssl_errors()
{
  curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
  return *this;
}

Curl& Curl::stderr(std::ostream& stream)
{
  stderr_pipe.reset(new FileToStreamPipe(stream));
  option(CURLOPT_STDERR, stderr_pipe->handle());
}

void Curl::flush_stderr()
{
  stderr_pipe->flush();
  stderr_pipe->read_all();
}

unsigned short Curl::perform()
{
  long status;

  if (headers)
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
  result = curl_easy_perform(handle);
  if (stderr_pipe)
    flush_stderr();
  if (result == CURLE_OK)
  {
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &status);
    return status;
  }
  return 0;
}

std::string Curl::error() const
{
  return curl_easy_strerror(result);
}

size_t Curl::header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
  Curl* self = reinterpret_cast<Curl*>(userdata);
  string_view contents(buffer, size * nitems);

  self->on_received_header(contents);
  return size * nitems;
}

size_t Curl::write_data(void* ptr, size_t size, size_t nmemb, void* userdata)
{
  Curl* self = reinterpret_cast<Curl*>(userdata);
  string_view contents(reinterpret_cast<char*>(ptr), size * nmemb);

  self->on_received_body(contents);
  return size * nmemb;
}
