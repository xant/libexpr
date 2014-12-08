#include "expression.h"
//#include "expression_loader.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include "ut.h"

static double random_num = 0;
double testOperand(void *user) {
    srandom(time(NULL));
    random_num = (double)(random()%8);
    return random_num;
}

int main(int argc, char **argv) {

    ut_init(basename(argv[0]));

    ut_testing("expression_create()");
    expression_t *expr = expression_create(EXPR_OP_SUM, 3, integer_operand_create(5), integer_operand_create(6), integer_operand_create(7));
    if (!expr)
        ut_failure("Can't create a new expression");
    else
        ut_success();

    ut_testing("expression_operand_create()");
    expression_operand_t *op1 = expression_operand_create(expr);
    if (!op1)
        ut_failure("Can't create a new expression operand");
    else
        ut_success();

    expression_t *expr_sin = expression_create(EXPR_OP_SIN, 1, float_operand_create(3.14/2));
    expression_t *expr2 = expression_create(EXPR_OP_SUB, 2, op1, expression_operand_create(expr_sin));

    ut_testing("expression_evaluate('%s') == 17", expression_dump(expr2));

    double result = expression_evaluate(expr2);
    //printf ("%s\n", expression_dump(expr2));
    //printf ("result: %.2lf\n", result);
    ut_validate_double(result, 17);
    expression_destroy(expr2);

    ut_testing("expression_evaluate('%s') == 1.0", expression_dump(expr_sin));
    result = expression_evaluate(expr_sin);
    ut_validate_double(result, 1.0);
    //printf ("result (sin): %.2lf\n", result);
    expression_destroy(expr_sin);

    /*
    struct stat st;
    if (stat("test/boolean_expression.xml", &st) == 0) {
        FILE *xmlfile = fopen("test/boolean_expression.xml", "r");
        char *buffer = malloc(st.st_size);
        fread(buffer, st.st_size, 1, xmlfile);

        ExpressionLoader *loader = CreateExpressionLoader();
        DefineExpressionFunction(loader, "testOperand", testOperand, NULL);
        if (!ParseExpressionXML(loader, buffer)) {
            printf("Can't parse expression xml : %s\n", buffer);
            exit(1);
        }
        free(buffer);
        Expression *loadedExpression = GetExpression(loader);
        printf ("\n%s\n\n", DumpExpression(loadedExpression));
        printf ("* True if random_num >= 4 && slept == 1 *\n\n");
        int i;
        int slept = 2;
        result = EvaluateExpression(loadedExpression);
        for (i = 0; i < 10; i++) {
            sleep(slept);
            result = EvaluateExpression(loadedExpression);
            printf ("result: %d (random num: %d, slept: %d)\n", (int)result, (int)random_num, slept);
            slept = i%2 + 1;
        }
        DestroyExpressionLoader(loader);
    }
    */

    expr = expression_create(EXPR_OP_AND, 3, integer_operand_create(5), integer_operand_create(1), integer_operand_create(0));
    expr2 = expression_create(EXPR_OP_OR, 2, expression_operand_create(expr), integer_operand_create(0));
    ut_testing("expression_evaluate('%s') == FALSE (0)", expression_dump(expr2));
    result = expression_evaluate(expr2);
    ut_validate_double(result, 1.0);
    //printf ("%s\n", expression_dump(expr2));
    //printf ("result: %d\n", (int)result);
    expression_destroy(expr2);
    expression_destroy(expr);

    ut_summary();
}
