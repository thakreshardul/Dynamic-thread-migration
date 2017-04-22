#include <stdio.h>
#include <unistd.h>

#include "dmtcp.h"


int main(int argc, char** argv)
{ 
  int dmtcp_enabled = dmtcp_is_enabled();

  if (!dmtcp_enabled) {
    printf("\n *** dmtcp_is_enabled: executable seems to not be running"
           " under dmtcp_launch.\n\n");
  }
  else
    printf("it works\n");

  return 0;
}
