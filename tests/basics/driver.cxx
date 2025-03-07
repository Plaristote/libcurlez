#include <sstream>
#include <stdexcept>
#include <iostream>

#include <libcurlez/version.hxx>
#include <libcurlez/curlez.hxx>

#undef NDEBUG
#include <cassert>

#define TEST_URL "http://curl.se"

int main ()
{
  using namespace std;

  // Basics.
  //
  {
    Curl curl;
    int status = curl
      .url(TEST_URL)
      .follow_redirects()
      .ignore_ssl_errors()
      .perform();
    assert(status == 200);
  }

  // Reading bodies.
  //
  {
    CurlReader query;
    int status = query
      .url(TEST_URL)
      .follow_redirects()
      .ignore_ssl_errors()
      .perform();

    assert(status == 200);
    assert(query.response_header("Content-Type").find("text/html") >= 0);
    assert(query.response_body().length() > 0);
  }

  // Throws exception when response header is called before headers are fetched
  //
  {
    CurlReader query;
    query.url(TEST_URL);

    try {
      query.response_header("Content-Type");
      assert(false);
    } catch (const std::runtime_error&) {
    }
  }

  // Properly implements move semantics
  //
  {
    CurlReader query = std::move(CurlReader().url(TEST_URL).follow_redirects().ignore_ssl_errors());

    assert(query.perform() == 200);
  }

  // Can redirect CURLOPT_STDERR to a stream
  //
  {
    ostringstream stream;
    Curl curl;
    int status = curl
      .verbose(true)
      .stderr(stream)
      .url(TEST_URL)
      .follow_redirects()
      .ignore_ssl_errors()
      .perform();
    assert(stream.str().length() > 0);
  }
}
