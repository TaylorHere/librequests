# librequests

a WIP TOY HTTP/1.0 client write in c with friendly API.

```shell
git clone --recursive https://github.com/TaylorHere/librequests.git <your-project>/vendors/librequests/
```
```cmake
add_subdirectory(vendors/librequests)
#set up your target here.
target_link_libraries(<your target> librequests)
```
```c
#include "librequests.h"

HTTPResponse response = requests("GET", "http://127.0.0.1/", "");

if (response.status_line.status_code >= 200){
    printf("%s", response.body);
}

HTTPResponse_drop(&response);
```
