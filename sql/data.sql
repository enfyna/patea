create table LESSON_CATEGORIES (
	category_id INTEGER PRIMARY KEY,
	name varchar(200)
);

create table LESSON_QUESTIONS (
	question_id INTEGER PRIMARY KEY,
	lesson_id int,
	question_pos int,
	question_type int,
	question varchar(200),
	choice1 varchar(50),
	choice2 varchar(50),
	choice3 varchar(50),
	choice4 varchar(50),
	answer int NOT NULL,
	FOREIGN KEY (lesson_id) REFERENCES LESSONS(lesson_id)
);

create table LESSONS (
	lesson_id INTEGER PRIMARY KEY,
	category_id int,
	title varchar(200),
	page_name varchar(50),
	question_count int,
	FOREIGN KEY (category_id) REFERENCES LESSON_CATEGORIES(category_id)
);

insert into LESSON_CATEGORIES values (NULL, 'Bilgisayarlar');
insert into LESSON_CATEGORIES values (NULL, 'Linux');
insert into LESSON_CATEGORIES values (NULL, 'Pardus');
insert into LESSON_CATEGORIES values (NULL, 'Terminal');
insert into LESSON_CATEGORIES values (NULL, 'Tarih');

insert into LESSONS values (NULL, 3, 'Pardus Nedir?', 'test_1', 3);

insert into LESSON_QUESTIONS values (NULL, 1, 0, 0, 'Pardus kim tarafindan gelistirilir ?','A: kimse','B: ben','C: tubitak','D: ankara', 2);
insert into LESSON_QUESTIONS values (NULL, 1, 1, 0, 'Pardus hangi linux tabanlidir ?','A: arch','B: ubuntu','C: mint','D: debian', 3);
insert into LESSON_QUESTIONS values (NULL, 1, 2, 0, 'Pardus hangi paket yoneticisini kullanir ?','A: pacman','B: apt','C: dnf','D: dpkg', 1);

insert into LESSONS values (NULL, 4, 'Terminal Nedir?', 'terminal_1', 7);

insert into LESSON_QUESTIONS values (NULL, 2, 0, 0, 'Hangi komut ile klasor olusturulur ?','A: makedirectory','B: makefolder','C: mkdir','D: mkfdr', 2);
insert into LESSON_QUESTIONS values (NULL, 2, 1, 1, 'mkdir komutunu kullanarak patea adinda bir klasor olustur.' || char(13) || char(10) || 'olusturulan klasorun yolu ~/patea olmali)' || char(13) || char(10),'test -d ~/patea','','','', 1);
insert into LESSON_QUESTIONS values (NULL, 2, 2, 0, 'Klasor icine girmek icin hangi komut kullanilir ?','A: ps','B: cd','C: ls','D: ed', 1);
insert into LESSON_QUESTIONS values (NULL, 2, 3, 0, 'Yeni bir dosya olusturmak icin hangi komut kullanilir ?','A: pkill','B: apt','C: pacman','D: touch', 3);
insert into LESSON_QUESTIONS values (NULL, 2, 4, 1, '1.cd komutunu kullanarak patea klasorune gir.' || char(13) || char(10) || '2.touch komutunu kullanarak metin.txt adinda bir dosya olustur' || char(13) || char(10),'test -f ~/patea/metin.txt','','','', 1);
insert into LESSON_QUESTIONS values (NULL, 2, 5, 0, 'Dosya yada klasor silmek icin hangi komut kullanilir ?','A: rm','B: ed','C: vi','D: ex', 0);
insert into LESSON_QUESTIONS values (NULL, 2, 6, 1, 'rm komutunu kullanarak patea klasorunu sil.' || char(13) || char(10),'! test -d ~/patea','','','', 1);

