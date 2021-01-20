Stopwatch
=========

Orignal Repo: https://github.com/mp3guy/Stopwatch


# Changes From Orignal Repo
1. Works on Windows
2. Added ImGui based viewer using glfw and opengl3

# Description
Easy to use simple benchmarking tool.

Sends UDP packets to localhost, which StopwatchViewer receives and displays stats on, including plots (credit to [Fiachra Matthews](http://www.linkedin.com/pub/fiachra-matthews/17/48b/a12) here). 

StopwatchViewer needs Qt4 to build. Simply include Stopwatch.h in whatever code you want to benchmark and use as such;

```cpp
#include <string>
#include <stdio.h>
#include "Stopwatch.h"

int main(int argc, char *argv[])
{
  //This stops duplicate timings on multiple runs
  Stopwatch::getInstance().setCustomSignature(32434);

  STOPWATCH("Timing1", 

  if(argc >= 1)
  {
    sleep(1);
  }

  );


  TICK("Timing2");

  while(argc > 0)
  {
    sleep(2);
    argc--;
  }

  TOCK("Timing2");

  Stopwatch::getInstance().sendAll();
}
```

Then just watch the stats in StopwatchViewer. 

Uses some code from the B-Human code release (http://www.b-human.de/). 

<p align="center">
  <img src="https://user-images.githubusercontent.com/29726242/105114723-8e06ff80-5b1b-11eb-806b-fa8b667a5921.png" alt="Stopwatch"/>
</p>
