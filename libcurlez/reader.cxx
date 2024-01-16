#include <libcurlez/curlez.hxx>

using namespace std;

std::string CurlReader::response_header(const std::string& name) const
{
  struct curl_header *type;
  CURLHcode code;

  code = curl_easy_header(get_handle(), name.c_str(), 0, CURLH_HEADER, -1, &type);
  switch (code)
  {
  case CURLHE_OK:
    return type->value;
  case CURLHE_NOT_BUILT_IN:
    throw std::logic_error("CurlReader::response_header: not available with this build of libcurl");
  case CURLHE_BAD_ARGUMENT:
    throw std::invalid_argument("CurlReader::response_header: invalid argument");
  case CURLHE_NOREQUEST:
  case CURLHE_NOHEADERS:
    throw std::runtime_error("CurlReader: called response header before headers were fetched");
  default:
    break ;
  }
  return std::string();
}

void CurlReader::on_received_body(std::string_view data)
{
  stored_body = data;
}

std::string_view CurlReader::response_body() const
{
  return std::string_view(stored_body.c_str(), stored_body.length());
}
