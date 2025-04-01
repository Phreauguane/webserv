#include <stdio.h>

int main() {
    printf("Content-type: text/html\n\n");
    printf("<html><body>");
    printf("<h1>Hello from C CGI!</h1>");
    printf("</body></html>");
    return 0;
}