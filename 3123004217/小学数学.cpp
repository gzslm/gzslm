#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_QUESTIONS 10000
#define MAX_EXPR_LEN 100

typedef struct {
    int numerator;
    int denominator;
} Fraction;

typedef struct {
    char expression[MAX_EXPR_LEN];
    Fraction answer;
} Question;

// ���Լ��
int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Լ�ַ���
void simplify(Fraction *f) {
    int common = gcd(abs(f->numerator), abs(f->denominator));
    f->numerator /= common;
    f->denominator /= common;
    if (f->denominator < 0) {
        f->numerator *= -1;
        f->denominator *= -1;
    }
}

// �����������
Fraction random_fraction(int range) {
    Fraction f;
    f.denominator = rand() % (range - 1) + 2; // ��ĸ����Ϊ2
    f.numerator = rand() % (f.denominator * (range - 1)) + 1;
    simplify(&f);
    return f;
}

// ���������Ȼ�������
Fraction random_number(int range, bool allow_fraction) {
    if (allow_fraction && rand() % 4 == 0) { // 25%�������ɷ���
        return random_fraction(range);
    } else {
        Fraction f;
        f.numerator = rand() % range;
        f.denominator = 1;
        return f;
    }
}

// ����ת�ַ���
void fraction_to_str(Fraction f, char *str) {
    if (f.denominator == 1) {
        sprintf(str, "%d", f.numerator);
    } else if (abs(f.numerator) > f.denominator) {
        int whole = f.numerator / f.denominator;
        int remainder = abs(f.numerator) % f.denominator;
        sprintf(str, "%d'%d/%d", whole, remainder, f.denominator);
    } else {
        sprintf(str, "%d/%d", f.numerator, f.denominator);
    }
}

// �ַ���ת����
Fraction str_to_fraction(const char *str) {
    Fraction f;
    int whole = 0, numerator = 0, denominator = 1;
    
    if (strchr(str, '\'') != NULL) {
        sscanf(str, "%d'%d/%d", &whole, &numerator, &denominator);
        f.numerator = whole * denominator + (whole >= 0 ? numerator : -numerator);
        f.denominator = denominator;
    } else if (strchr(str, '/') != NULL) {
        sscanf(str, "%d/%d", &numerator, &denominator);
        f.numerator = numerator;
        f.denominator = denominator;
    } else {
        sscanf(str, "%d", &numerator);
        f.numerator = numerator;
        f.denominator = 1;
    }
    
    simplify(&f);
    return f;
}

// �����ӷ�
Fraction add(Fraction a, Fraction b) {
    Fraction result;
    result.numerator = a.numerator * b.denominator + b.numerator * a.denominator;
    result.denominator = a.denominator * b.denominator;
    simplify(&result);
    return result;
}

// ��������
Fraction subtract(Fraction a, Fraction b) {
    Fraction result;
    result.numerator = a.numerator * b.denominator - b.numerator * a.denominator;
    result.denominator = a.denominator * b.denominator;
    simplify(&result);
    return result;
}

// �����˷�
Fraction multiply(Fraction a, Fraction b) {
    Fraction result;
    result.numerator = a.numerator * b.numerator;
    result.denominator = a.denominator * b.denominator;
    simplify(&result);
    return result;
}

// ��������
Fraction divide(Fraction a, Fraction b) {
    Fraction result;
    result.numerator = a.numerator * b.denominator;
    result.denominator = a.denominator * b.numerator;
    simplify(&result);
    return result;
}

// ���ɱ��ʽ�ʹ�
void generate_expression(Question *q, int range, int operators_left, bool in_parentheses) {
    if (operators_left == 0) {
        Fraction num = random_number(range, true);
        char num_str[20];
        fraction_to_str(num, num_str);
        strcat(q->expression, num_str);
        q->answer = num;
        return;
    }

    bool use_parentheses = in_parentheses ? false : (rand() % 2 == 0 && operators_left > 1);
    if (use_parentheses) {
        strcat(q->expression, "(");
    }

    // ���ӱ��ʽ
    int left_operators = rand() % operators_left;
    if (left_operators == operators_left) left_operators--;
    generate_expression(q, range, left_operators, use_parentheses);

    // �����
    char op;
    Fraction temp_answer;
    switch (rand() % 4) {
        case 0: op = '+'; break;
        case 1: op = '-'; break;
        case 2: op = '*'; break;
        case 3: op = '/'; break;
    }
    
    // ��������ͳ���������
    bool valid = false;
    Fraction right_num;
    char right_str[20];
    int attempts = 0;
    
    do {
        attempts++;
        if (attempts > 10) { // ��������ѭ��
            op = (op == '-' || op == '/') ? '+' : op;
        }
        
        // �������ӱ��ʽ
        char temp_expr[MAX_EXPR_LEN] = "";
        Question temp_q;
        strcpy(temp_q.expression, temp_expr);
        generate_expression(&temp_q, range, operators_left - left_operators - 1, use_parentheses);
        right_num = temp_q.answer;
        strcpy(right_str, temp_q.expression);
        
        if (op == '-') {
            valid = (q->answer.numerator * right_num.denominator >= 
                    right_num.numerator * q->answer.denominator);
        } else if (op == '/') {
            valid = (right_num.numerator != 0);
        } else {
            valid = true;
        }
    } while (!valid);
    
    // �������������ӱ��ʽ
    char op_with_spaces[4] = " ";
    op_with_spaces[1] = op;
    op_with_spaces[2] = ' ';
    strcat(q->expression, op_with_spaces);
    strcat(q->expression, right_str);
    
    // �����
    switch (op) {
        case '+': q->answer = add(q->answer, right_num); break;
        case '-': q->answer = subtract(q->answer, right_num); break;
        case '*': q->answer = multiply(q->answer, right_num); break;
        case '/': q->answer = divide(q->answer, right_num); break;
    }
    
    if (use_parentheses) {
        strcat(q->expression, ")");
    }
}

// �����Ŀ�Ƿ��ظ�
bool is_duplicate(Question *questions, int count, Question *new_q) {
    // �򻯼�飺�Ƚϴ𰸺ͱ��ʽ����
    for (int i = 0; i < count; i++) {
        if (questions[i].answer.numerator == new_q->answer.numerator &&
            questions[i].answer.denominator == new_q->answer.denominator &&
            strlen(questions[i].expression) == strlen(new_q->expression)) {
            // �����ӵļ��������������
            return true;
        }
    }
    return false;
}

// ������Ŀ
void generate_questions(int count, int range, const char *exercise_file, const char *answer_file) {
    Question questions[MAX_QUESTIONS];
    FILE *ef = fopen(exercise_file, "w");
    FILE *af = fopen(answer_file, "w");
    
    if (!ef || !af) {
        printf("�޷�������ļ�\n");
        return;
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < count; i++) {
        Question q;
        strcpy(q.expression, "");
        
        int operators = rand() % 3 + 1; // 1-3�������
        generate_expression(&q, range, operators, false);
        
        // ȷ�����ظ�
        while (is_duplicate(questions, i, &q)) {
            strcpy(q.expression, "");
            generate_expression(&q, range, operators, false);
        }
        
        questions[i] = q;
        
        // д����Ŀ�ļ�
        fprintf(ef, "%d. %s =\n", i + 1, q.expression);
        
        // д����ļ�
        char answer_str[20];
        fraction_to_str(q.answer, answer_str);
        fprintf(af, "%d. %s\n", i + 1, answer_str);
    }
    
    fclose(ef);
    fclose(af);
}

// ����
void check_answers(const char *exercise_file, const char *answer_file, const char *grade_file) {
    FILE *ef = fopen(exercise_file, "r");
    FILE *af = fopen(answer_file, "r");
    FILE *gf = fopen(grade_file, "w");
    
    if (!ef || !af || !gf) {
        printf("�޷�������/����ļ�\n");
        return;
    }
    
    int correct_count = 0, wrong_count = 0;
    int correct_indices[MAX_QUESTIONS], wrong_indices[MAX_QUESTIONS];
    char line[256];
    int index;
    
    while (fgets(line, sizeof(line), ef)) {
        sscanf(line, "%d.", &index);
        
        char answer_line[256];
        fgets(answer_line, sizeof(answer_line), af);
        
        // ��ȡ��ȷ��
        char correct_answer[20];
        sscanf(answer_line, "%*d. %s", correct_answer);
        Fraction correct = str_to_fraction(correct_answer);
        
        // ��ȡ�û���
        char *eq_pos = strchr(line, '=');
        if (!eq_pos || *(eq_pos + 1) == '\n') {
            // û�д�
            wrong_indices[wrong_count++] = index;
            continue;
        }
        
        char user_answer[20];
        sscanf(eq_pos + 1, "%s", user_answer);
        Fraction user = str_to_fraction(user_answer);
        
        if (user.numerator == correct.numerator && user.denominator == correct.denominator) {
            correct_indices[correct_count++] = index;
        } else {
            wrong_indices[wrong_count++] = index;
        }
    }
    
    // д��ɼ��ļ�
    fprintf(gf, "Correct: %d (", correct_count);
    for (int i = 0; i < correct_count; i++) {
        fprintf(gf, "%d", correct_indices[i]);
        if (i < correct_count - 1) fprintf(gf, ", ");
    }
    fprintf(gf, ")\n");
    
    fprintf(gf, "Wrong: %d (", wrong_count);
    for (int i = 0; i < wrong_count; i++) {
        fprintf(gf, "%d", wrong_indices[i]);
        if (i < wrong_count - 1) fprintf(gf, ", ");
    }
    fprintf(gf, ")\n");
    
    fclose(ef);
    fclose(af);
    fclose(gf);
}

// ��ӡ������Ϣ
void print_help() {
    printf("ʹ�÷���:\n");
    printf("������Ŀ: program -n <��Ŀ����> -r <��ֵ��Χ>\n");
    printf("����: program -e <��Ŀ�ļ�> -a <���ļ�>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }
    
    if (strcmp(argv[1], "-n") == 0 && argc >= 5 && strcmp(argv[3], "-r") == 0) {
        int count = atoi(argv[2]);
        int range = atoi(argv[4]);
        
        if (count <= 0 || range <= 0) {
            printf("��Ч�Ĳ���ֵ\n");
            return 1;
        }
        
        generate_questions(count, range, "Exercises.txt", "Answers.txt");
        printf("������ %d ����Ŀ����Χ�� %d ����\n", count, range);
    } else if (strcmp(argv[1], "-e") == 0 && argc >= 5 && strcmp(argv[3], "-a") == 0) {
        check_answers(argv[2], argv[4], "Grade.txt");
        printf("�Ѽ��𰸣���������� Grade.txt\n");
    } else {
        print_help();
        return 1;
    }
    
    return 0;
}
