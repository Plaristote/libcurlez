# libcurlez

A C++ library wrapping libcurl's easy API, allowing you
to easily perform synchronous HTTP queries.

## Example

Simple query:

```c++
#include <libcurlez/curlez.hxx>
#include <iostream>

int main()
{
  Curl query;
  unsigned short status;

  query.url("http://google.fr")
       .follow_redirects();
  switch (status = query.perform())
  {
  case 0:
    std::cout << "CURL error: " << query.error() << std::endl;
    break ;
  default:
    std::cout << "Received status: " << status;
    break ;
  }
  return 0;
}
```

Reading response headers and body:

```c++
#include <libcurlez/curlez.hxx>
#include <iostream>

int main()
{
  CurlReader query;

  query.url("http://google.fr")
       .follow_redirects();
  if (query.perform() == 200)
  {
    std::cout << "Content-Type = " << query.response_header("Content-Type")
              << std::endl << "--" << std::endl
              << query.response_body() << std::endl;
    return 0;
  }
  return -1;
}
```

Sending queries with custom method, header and body:

```c++
#include <libcurlez/curlez.hxx>
#include <iostream>

int main()
{
  Curl query;

  query.method("PUT")
       .url("http://google.fr")
       .header("Content-Type", "application/json")
       .body("{ \"json\": 42 }")
       .follow_redirects();
  return query.perform() == 200;
}
```
