#pragma once
#include <libcurlez/export.hxx>
#include <curl/curl.h>
#include <string_view>
#include <string>
#include <stdexcept>

class LIBCURLEZ_SYMEXPORT Curl
{
public:
  Curl();
  virtual ~Curl();

  Curl(const Curl&) = delete;
  Curl(Curl&& b);

  unsigned short perform();
  Curl& method(const std::string&);
  Curl& url(const std::string&);
  Curl& body(const std::string&);
  Curl& body(std::string_view);
  Curl& header(std::string_view, std::string_view);
  Curl& header(std::string_view, const std::string&);
  Curl& follow_redirects();
  Curl& ignore_ssl_errors();

  template<typename PARAMETER_TYPE>
  Curl& header(std::string_view name, PARAMETER_TYPE value)
  {
    return header(name, std::to_string(value));
  }

  Curl& option(CURLoption flag, const std::string& parameter)
  {
    return option(flag, parameter.c_str());
  }

  template<typename PARAMETER_TYPE>
  Curl& option(CURLoption option, PARAMETER_TYPE parameter)
  {
    CURLcode result = curl_easy_setopt(handle, option, parameter);

    if (result != CURLE_OK)
      throw std::invalid_argument("Curl::set_opt: invalid parameter");
    return *this;
  }

  CURL* get_handle() const
  {
    return handle;
  }

  std::string error() const;
  CURLcode error_code() const { return result; }

protected:
  virtual void on_received_header(std::string_view) {}
  virtual void on_received_body(std::string_view) {}

private:
  static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata);
  static size_t write_data(void* ptr, size_t size, size_t nmemb, void* userdata);

  CURL* handle;
  CURLcode result;
  struct curl_slist* headers = nullptr;
};

class LIBCURLEZ_SYMEXPORT CurlReader : public Curl
{
  std::string stored_body;
public:
  CurlReader() {}
  CurlReader(Curl&& b) : Curl(std::move(b)) {}

  std::string response_header(const std::string&) const;
  std::string_view response_body() const;

private:
  void on_received_body(std::string_view data) override;
};
