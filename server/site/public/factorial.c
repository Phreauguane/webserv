#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_html_header() {
    printf("Content-type: text/html\n\n");
    printf("<!DOCTYPE html>\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("    <title>Calculateur de Factorielle</title>\n");
    printf("    <link rel=\"stylesheet\" href=\"/site/assets/css/style.css\">\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("    <div class=\"container\">\n");
    printf("        <h1>Calculateur de Factorielle</h1>\n");
}

void print_html_footer() {
    printf("    </div>\n");
    printf("</body>\n");
    printf("</html>\n");
}

unsigned long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    print_html_header();

    char *query_string = getenv("QUERY_STRING");
    char *number_str = NULL;
    
    if (query_string && strstr(query_string, "number=")) {
        number_str = strchr(query_string, '=') + 1;
        int number = atoi(number_str);
        
        if (number >= 0 && number <= 20) {
            unsigned long result = factorial(number);
            printf("<p>La factorielle de %d est : %lu</p>\n", number, result);
        } else {
            printf("<p class=\"error\">Veuillez entrer un nombre entre 0 et 20</p>\n");
        }
    }

    printf("<form method=\"get\">\n");
    printf("    <div class=\"form-group\">\n");
    printf("        <label for=\"number\">Entrez un nombre (0-20):</label>\n");
    printf("        <input type=\"number\" name=\"number\" id=\"number\" min=\"0\" max=\"20\" required>\n");
    printf("    </div>\n");
    printf("    <button type=\"submit\" class=\"btn\">Calculer</button>\n");
    printf("</form>\n");

    print_html_footer();
    return 0;
} 