#include <string.h>
#include <glib.h>

#include "lesson.h"

char* lesson_ids[LESSON_COUNT] = {
    [TEST_1] = "test_1",
    [TERMINAL_1] = "terminal_1",
};

Lesson* tests[TEST_COUNT] = { 0 };

void lesson_init(void)
{
    Lesson* test_1 = calloc(1, sizeof(Lesson));

    test_1->title = "Pardus Nedir?";
    test_1->total_questions = 3;
    test_1->current_question = 0;

    strcpy(test_1->questions[0].question, "Pardus kim tarafindan gelistirilir ?");
    strcpy(test_1->questions[0].a, "A: kimse");
    strcpy(test_1->questions[0].b, "B: ben");
    strcpy(test_1->questions[0].c, "C: tubitak");
    strcpy(test_1->questions[0].d, "D: ankara");
    test_1->questions[0].answer = 2;
    test_1->questions[0].type = QUESTION_TEST;

    strcpy(test_1->questions[1].question, "Pardus hangi linux tabanlidir ?");
    strcpy(test_1->questions[1].a, "A: arch");
    strcpy(test_1->questions[1].b, "B: ubuntu");
    strcpy(test_1->questions[1].c, "C: mint");
    strcpy(test_1->questions[1].d, "D: debian");
    test_1->questions[1].answer = 3;
    test_1->questions[1].type = QUESTION_TEST;

    strcpy(test_1->questions[2].question, "Pardus hangi paket yoneticisini kullanir ?");
    strcpy(test_1->questions[2].a, "A: pacman");
    strcpy(test_1->questions[2].b, "B: apt");
    strcpy(test_1->questions[2].c, "C: dnf");
    strcpy(test_1->questions[2].d, "D: dpkg");
    test_1->questions[2].answer = 1;
    test_1->questions[2].type = QUESTION_TEST;

    tests[TEST_1] = test_1;

    Lesson* terminal_1 = calloc(1, sizeof(Lesson));

    terminal_1->title = "Terminal Nedir?";
    terminal_1->total_questions = 7;
    terminal_1->current_question = 0;

    strcpy(terminal_1->questions[0].question, "Hangi komut ile klasor olusturulur ?");
    strcpy(terminal_1->questions[0].a, "A: makedirectory");
    strcpy(terminal_1->questions[0].b, "B: makefolder");
    strcpy(terminal_1->questions[0].c, "C: mkdir");
    strcpy(terminal_1->questions[0].d, "D: mkfdr");
    terminal_1->questions[0].answer = 2;
    terminal_1->questions[0].type = QUESTION_TEST;

    strcpy(terminal_1->questions[1].question, "mkdir komutunu kullanarak patea adinda bir klasor olustur.\r\n(olusturulan klasorun yolu ~/patea olmali)\r\n");
    strcpy(terminal_1->questions[1].a, "test -d ~/patea");
    strcpy(terminal_1->questions[1].b, "");
    strcpy(terminal_1->questions[1].c, "");
    strcpy(terminal_1->questions[1].d, "");
    terminal_1->questions[1].answer = 1;
    terminal_1->questions[1].type = QUESTION_TERMINAL;

    strcpy(terminal_1->questions[2].question, "Klasor icine girmek icin hangi komut kullanilir ?");
    strcpy(terminal_1->questions[2].a, "A: ps");
    strcpy(terminal_1->questions[2].b, "B: cd");
    strcpy(terminal_1->questions[2].c, "C: ls");
    strcpy(terminal_1->questions[2].d, "D: ed");
    terminal_1->questions[2].answer = 1;
    terminal_1->questions[2].type = QUESTION_TEST;

    strcpy(terminal_1->questions[3].question, "Yeni bir dosya olusturmak icin hangi komut kullanilir ?");
    strcpy(terminal_1->questions[3].a, "A: pkill");
    strcpy(terminal_1->questions[3].b, "B: apt");
    strcpy(terminal_1->questions[3].c, "C: pacman");
    strcpy(terminal_1->questions[3].d, "D: touch");
    terminal_1->questions[3].answer = 3;
    terminal_1->questions[3].type = QUESTION_TEST;

    strcpy(terminal_1->questions[4].question, "1.cd komutunu kullanarak patea klasorune gir.\r\n2.touch komutunu kullanarak metin.txt adinda bir dosya olustur\r\n");
    strcpy(terminal_1->questions[4].a, "test -f ~/patea/metin.txt");
    strcpy(terminal_1->questions[4].b, "");
    strcpy(terminal_1->questions[4].c, "");
    strcpy(terminal_1->questions[4].d, "");
    terminal_1->questions[4].answer = 1;
    terminal_1->questions[4].type = QUESTION_TERMINAL;

    strcpy(terminal_1->questions[5].question, "Dosya yada klasor silmek icin hangi komut kullanilir ?");
    strcpy(terminal_1->questions[5].a, "A: rm");
    strcpy(terminal_1->questions[5].b, "B: ed");
    strcpy(terminal_1->questions[5].c, "C: vi");
    strcpy(terminal_1->questions[5].d, "D: ex");
    terminal_1->questions[5].answer = 1;
    terminal_1->questions[5].type = QUESTION_TEST;

    strcpy(terminal_1->questions[6].question, "rm komutunu kullanarak patea klasorunu sil.\r\n");
    strcpy(terminal_1->questions[6].a, "! test -d ~/patea");
    strcpy(terminal_1->questions[6].b, "");
    strcpy(terminal_1->questions[6].c, "");
    strcpy(terminal_1->questions[6].d, "");
    terminal_1->questions[6].answer = 1;
    terminal_1->questions[6].type = QUESTION_TERMINAL;
    tests[TERMINAL_1] = terminal_1;
}

enum LESSONS lesson_get_from_page(const char* page_name)
{
    // removing "lesson_" prefix
    const char* lesson_name = page_name + 7;

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
