#include <string.h>
#include <glib.h>

#include "lesson.h"

char* lesson_ids[LESSON_COUNT] = {
    [TEST_1] = "test_1",
    [TERMINAL_1] = "terminal_1",
};

Lesson* tests[LESSON_COUNT] = { 0 };

void lesson_init(void)
{
    Lesson* test_1 = calloc(1, sizeof(Lesson) + 3 * sizeof(LessonQuestion));

    test_1->title = "Pardus Nedir?";
    test_1->category = CATEGORY_PARDUS;
    test_1->total_questions = 3;
    test_1->current_question = 0;

    strcpy(test_1->questions[0].question, "Pardus kim tarafindan gelistirilir ?");
    strcpy(test_1->questions[0].choice[0], "A: kimse");
    strcpy(test_1->questions[0].choice[1], "B: ben");
    strcpy(test_1->questions[0].choice[2], "C: tubitak");
    strcpy(test_1->questions[0].choice[3], "D: ankara");
    test_1->questions[0].answer = 2;
    test_1->questions[0].type = QUESTION_TEST;

    strcpy(test_1->questions[1].question, "Pardus hangi linux tabanlidir ?");
    strcpy(test_1->questions[1].choice[0], "A: arch");
    strcpy(test_1->questions[1].choice[1], "B: ubuntu");
    strcpy(test_1->questions[1].choice[2], "C: mint");
    strcpy(test_1->questions[1].choice[3], "D: debian");
    test_1->questions[1].answer = 3;
    test_1->questions[1].type = QUESTION_TEST;

    strcpy(test_1->questions[2].question, "Pardus hangi paket yoneticisini kullanir ?");
    strcpy(test_1->questions[2].choice[0], "A: pacman");
    strcpy(test_1->questions[2].choice[1], "B: apt");
    strcpy(test_1->questions[2].choice[2], "C: dnf");
    strcpy(test_1->questions[2].choice[3], "D: dpkg");
    test_1->questions[2].answer = 1;
    test_1->questions[2].type = QUESTION_TEST;

    tests[TEST_1] = test_1;

    Lesson* terminal_1 = calloc(1, sizeof(Lesson) + 7 * sizeof(LessonQuestion));

    terminal_1->title = "Terminal Nedir?";
    terminal_1->category = CATEGORY_TERMINAL;
    terminal_1->total_questions = 7;
    terminal_1->current_question = 0;

    strcpy(terminal_1->questions[0].question, "Hangi komut ile klasor olusturulur ?");
    strcpy(terminal_1->questions[0].choice[0], "A: makedirectory");
    strcpy(terminal_1->questions[0].choice[1], "B: makefolder");
    strcpy(terminal_1->questions[0].choice[2], "C: mkdir");
    strcpy(terminal_1->questions[0].choice[3], "D: mkfdr");
    terminal_1->questions[0].answer = 2;
    terminal_1->questions[0].type = QUESTION_TEST;

    strcpy(terminal_1->questions[1].question, "mkdir komutunu kullanarak patea adinda bir klasor olustur.\r\n(olusturulan klasorun yolu ~/patea olmali)\r\n");
    strcpy(terminal_1->questions[1].choice[0], "test -d ~/patea");
    strcpy(terminal_1->questions[1].choice[1], "");
    strcpy(terminal_1->questions[1].choice[2], "");
    strcpy(terminal_1->questions[1].choice[3], "");
    terminal_1->questions[1].answer = 1;
    terminal_1->questions[1].type = QUESTION_TERMINAL;

    strcpy(terminal_1->questions[2].question, "Klasor icine girmek icin hangi komut kullanilir ?");
    strcpy(terminal_1->questions[2].choice[0], "A: ps");
    strcpy(terminal_1->questions[2].choice[1], "B: cd");
    strcpy(terminal_1->questions[2].choice[2], "C: ls");
    strcpy(terminal_1->questions[2].choice[3], "D: ed");
    terminal_1->questions[2].answer = 1;
    terminal_1->questions[2].type = QUESTION_TEST;

    strcpy(terminal_1->questions[3].question, "Yeni bir dosya olusturmak icin hangi komut kullanilir ?");
    strcpy(terminal_1->questions[3].choice[0], "A: pkill");
    strcpy(terminal_1->questions[3].choice[1], "B: apt");
    strcpy(terminal_1->questions[3].choice[2], "C: pacman");
    strcpy(terminal_1->questions[3].choice[3], "D: touch");
    terminal_1->questions[3].answer = 3;
    terminal_1->questions[3].type = QUESTION_TEST;

    strcpy(terminal_1->questions[4].question, "1.cd komutunu kullanarak patea klasorune gir.\r\n2.touch komutunu kullanarak metin.txt adinda bir dosya olustur\r\n");
    strcpy(terminal_1->questions[4].choice[0], "test -f ~/patea/metin.txt");
    strcpy(terminal_1->questions[4].choice[1], "");
    strcpy(terminal_1->questions[4].choice[2], "");
    strcpy(terminal_1->questions[4].choice[3], "");
    terminal_1->questions[4].answer = 1;
    terminal_1->questions[4].type = QUESTION_TERMINAL;

    strcpy(terminal_1->questions[5].question, "Dosya yada klasor silmek icin hangi komut kullanilir ?");
    strcpy(terminal_1->questions[5].choice[0], "A: rm");
    strcpy(terminal_1->questions[5].choice[1], "B: ed");
    strcpy(terminal_1->questions[5].choice[2], "C: vi");
    strcpy(terminal_1->questions[5].choice[3], "D: ex");
    terminal_1->questions[5].answer = 0;
    terminal_1->questions[5].type = QUESTION_TEST;

    strcpy(terminal_1->questions[6].question, "rm komutunu kullanarak patea klasorunu sil.\r\n");
    strcpy(terminal_1->questions[6].choice[0], "! test -d ~/patea");
    strcpy(terminal_1->questions[6].choice[1], "");
    strcpy(terminal_1->questions[6].choice[2], "");
    strcpy(terminal_1->questions[6].choice[3], "");
    terminal_1->questions[6].answer = 1;
    terminal_1->questions[6].type = QUESTION_TERMINAL;

    tests[TERMINAL_1] = terminal_1;
}

enum LESSONS lesson_get_from_name(const char* name)
{
    // removing "lesson_" prefix
    const char* lesson_name = name + 7;

    for (int i = 0; i < LESSON_COUNT; i++) {
        if (0 == strcmp(lesson_ids[i], lesson_name)) {
            return i;
        }
    }

    return LESSON_NOT_SELECTED;
}

Lesson* lesson_get_from_enum(enum LESSONS en)
{
    return tests[en];
}
